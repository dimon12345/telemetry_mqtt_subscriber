#include "MqttProcessor.hpp"

#include <iostream>

using namespace Telemetry::Domain;

namespace Telemetry::Infrastructure {

MqttProcessor::~MqttProcessor() {
    stop();
}

void MqttProcessor::start() {
    if (running_) {
        return;
    }
    running_ = true;

    worker_thread_ = std::thread(&MqttProcessor::process_loop, this);
}

void MqttProcessor::stop() {
    if (!running_) {
        return;
    }
    running_ = false;

    cv_.notify_all();

    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

void MqttProcessor::pushMessage(const TelemetryMessage &message) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(message);
    }
    cv_.notify_one();
}

void MqttProcessor::process_loop() {
    while (running_) {
        TelemetryMessage message;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return !queue_.empty() || !running_; });

            if (!running_ && queue_.empty()) {
                break;
            }

            message = queue_.front();
            queue_.pop();
        }

        try {
            repository_->save(message);
        } 
        catch (const std::exception& e) {
            std::cerr << "[Processor] Error: " << e.what() << std::endl;
        }
    }
}

} // namespace Telemetry::Infrastructure
