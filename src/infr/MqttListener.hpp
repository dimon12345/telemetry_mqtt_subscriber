#pragma once

#include <memory>

#include <mqtt/async_client.h>

#include "MqttProcessor.hpp"
#include "configs/MqttConfig.hpp"

namespace Telemetry::Infrastructure {

class MqttListener : public mqtt::callback {
public:
    MqttListener(
            std::shared_ptr<MqttProcessor> processor, const MqttConfig &config)
        : processor_(processor), config_(config) {}
    ~MqttListener();

    void connect();
    void disconnect();
    bool isSubscribed();

    void connected(const std::string &cause) override;
    void connection_lost(const std::string &cause) override;
    void message_arrived(mqtt::const_message_ptr msg) override;

private:
    std::mutex mutex_;
    mqtt::token_ptr subscribe_token_;
    std::shared_ptr<MqttProcessor> processor_;
    const MqttConfig &config_;
    std::unique_ptr<mqtt::async_client> mqtt_client_;

    float parseTemperature(mqtt::const_message_ptr msg);
};

} // namespace Telemetry::Infrastructure
