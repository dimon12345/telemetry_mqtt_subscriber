#pragma once

#include <iostream>


#include "../MqttMessageConsumer.h"

class ConsoleOutputConsumer : public MqttMessageConsumer {
public:

    void onMessageArrived(const MqttMessage &message);
};
