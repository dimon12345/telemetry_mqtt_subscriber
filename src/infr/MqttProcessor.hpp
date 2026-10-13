#pragma once

#include <atomic>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <thread>

#include "core/TelemetryMessage.hpp"

namespace Telemetry::Infrastructure {

class MqttProcessor {
public:
    explicit MqttProcessor(std::shared_ptr<Telemetry::Domain::ITelemetryRepository> repository)
            : repository_(repository) {}

    ~MqttProcessor();
    void start();
    void stop();
    void pushMessage(const Telemetry::Domain::TelemetryMessage &message);

private:
    std::shared_ptr<Telemetry::Domain::ITelemetryRepository> repository_;

    std::queue<Telemetry::Domain::TelemetryMessage> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;

    std::thread worker_thread_;
    std::atomic<bool> running_{false};

    void process_loop();
};

} // namespace Telemetry::Infrastructure
