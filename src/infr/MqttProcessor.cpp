#include "MqttProcessor.hpp"

#include <cstring>
#include <iostream>

#include "util/BackoffTimer.hpp"

using namespace Telemetry::Domain;

namespace Telemetry::Infrastructure {

MqttProcessor::~MqttProcessor() { stop(); }

void MqttProcessor::start() {
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (running_) {
            return;
        }
        running_ = true;
    }

    worker_thread_ = std::thread(&MqttProcessor::process_loop, this);
}

void MqttProcessor::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!running_) {
            return;
        }
        running_ = false;
    }

    cv_.notify_all();

    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

void MqttProcessor::pushMessage(const TelemetryMessage &message) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        constexpr int MAX_QUEUE_SIZE = 1024;
        if (queue_.size() >= MAX_QUEUE_SIZE) {
            std::cerr << "[Processor] Buffer overflow. Old message dropped."
                      << std::endl;
            queue_.pop();
        }
        queue_.push(message);
    }
    cv_.notify_one();
}

void MqttProcessor::process_loop() {
    const int MIN_SLEEP_S = 1;
    const int MAX_SLEEP_S = 60;

    BackoffTimer bacckoff_timer{std::chrono::seconds{MIN_SLEEP_S},
            std::chrono::seconds{MAX_SLEEP_S}};

    for (;;) {
        TelemetryMessage message;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return !queue_.empty() || !running_; });

            if (queue_.empty() && !running_) {
                break;
            }

            message = queue_.front();
            queue_.pop();
        }

        for (;;) {
            try {
                repository_->save(message);
                bacckoff_timer.onSuccess();
                break;
            } catch (const TelemetryConnectionError &e) {
                const char *msg = e.what();
                std::cerr << "[Processor] Connection error: " << msg;
                size_t len = std::strlen(msg);
                if (len != 0 && msg[len - 1] != '\n') {
                    std::cerr << std::endl;
                }

                std::chrono::duration delay = bacckoff_timer.getNextDelay();
                std::unique_lock<std::mutex> lock(mutex_);
                if (!running_) {
                    break;
                }
                cv_.wait_for(lock, delay, [this] { return !running_; });
            } catch (const std::exception &e) {
                std::cerr << "[Processor] Error: " << e.what() << std::endl;
                break;
            }

            std::lock_guard<std::mutex> lock(mutex_);
            if (queue_.empty() && !running_) {
                break;
            }
        }
    }
}

} // namespace Telemetry::Infrastructure
