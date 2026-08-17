#pragma once

#include "MqttMessageConsumer.h"

class ConsoleOutputConsumer : public MqttMessageConsumer {
    void onMessageArrived(const MqttMessage &message);
};
