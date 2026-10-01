#include <iostream>


#pragma once

#include <memory>
#include <mutex>

#include "mqtt/async_client.h"
#include "MqttMessageConsumer.h"

class MqttSubscriber : public mqtt::callback {
public:
    MqttSubscriber(const class Config &config)
            : config_(config) { std::cout << "MqttSubscriber constructor" << std::endl;};
    ~MqttSubscriber() { std::cout << "MqttSubscriber destructor" << std::endl;};

    void subscribe(std::weak_ptr<MqttMessageConsumer> subscriber);

    void start();
    void stop();

    void message_arrived(mqtt::const_message_ptr msg) override;

private:
    const class Config &config_;
    std::unique_ptr<mqtt::async_client> client_;

    std::vector<std::weak_ptr<MqttMessageConsumer> > subscribers_;
    std::mutex subscribers_mutex_;
};
