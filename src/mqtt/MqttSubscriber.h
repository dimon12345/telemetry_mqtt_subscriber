#pragma once

#include <memory>
#include <mutex>

#include "mqtt/async_client.h"
#include "MqttMessageConsumer.h"

class MqttSubscriber : public mqtt::callback {
public:
    MqttSubscriber(const class Config &config) : config_(config) {}
    void subscribe(std::weak_ptr<MqttMessageConsumer> subscriber);
    int run();

    void message_arrived(mqtt::const_message_ptr msg) override;

private:
    const class Config &config_;
    std::vector<std::weak_ptr<MqttMessageConsumer> > subscribers_;
    std::mutex subscribers_mutex_;
};
