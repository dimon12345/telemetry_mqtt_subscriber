#pragma once

#include <atomic>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <optional>
#include <sstream>

#include "common/ThreadSafeQueue.hpp"
#include "core/Sensors.hpp"

using namespace Telemetry::Domain;
using namespace Telemetry::Common;

namespace Telemetry::Infrastructure {

class MqttTelemetryService : public mqtt::callback {
public:
    explicit MqttTelemetryService(std::shared_ptr<ISensorValueService> service)
            : sensor_value_service_(std::move(service)) {
    }

    ~MqttTelemetryService() override {
        stop();
    }

    void start() {
        if (!is_running_.exchange(true)) {
            worker_thread_ = std::thread(&MqttTelemetryService::processQueueLoop, this);
        }
    }

    void stop() {
        if (is_running_.exchange(false)) {
            queue_.abort();
            if (worker_thread_.joinable()) {
                worker_thread_.join();
            }
        }
    }

    void message_arrived(mqtt::const_message_ptr msg) override {
        queue_.push(std::move(msg));
    }

private:
    std::shared_ptr <ISensorValueService> sensor_value_service_;
    std::atomic<bool> is_running_{false};
    std::thread worker_thread_;
    ThreadSafeQueue<mqtt::const_message_ptr> queue_;

    void processQueueLoop() {
        while (is_running_) {
            auto msg_opt = queue_.pop();
            if (!msg_opt.has_value()) {
                break;
            }
            processSingleMessage(std::move(msg_opt.value()));
        }
    }

    void processSingleMessage(mqtt::const_message_ptr msg) {
        try {
            const std::string &topic = msg->get_topic();
            const std::string &payload = msg->get_payload_str();

            std::string sensor_name = parseSensorName(topic);
            float temperature = 0.0;
            std::stringstream ss(payload);
            if (!(ss >> temperature)) {
                std::cerr << "[Worker] Parsing error: '" << payload << "' on " << topic << std::endl;
                return;
            }
            sensor_value_service_->save(SensorValue{std::move(sensor_name), temperature});
        } catch (const std::exception &e) {
            std::cerr << "[Worker] Error processing message: " << e.what() << std::endl;
        }
    }

    std::string parseSensorName(std::string_view topic) const {
        size_t last_slash = topic.find_last_of('/');
        if (last_slash != std::string_view::npos && last_slash < topic.length() - 1) {
            return std::string(topic.substr(last_slash + 1));
        }
        return std::string(topic);
    }
};

} // namespace Telemetry::Infrastructure
