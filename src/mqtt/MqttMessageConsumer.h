#pragma once

#include <string>
#include <vector>

struct MqttMessage {
    std::string topic;
    std::vector<std::string> text_fields;
};

class MqttMessageConsumer {
public:
    virtual void onMessageArrived(const MqttMessage &message) = 0;
};
