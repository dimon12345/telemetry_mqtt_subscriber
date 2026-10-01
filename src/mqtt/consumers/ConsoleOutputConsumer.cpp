#include <iostream>

#include "ConsoleOutputConsumer.h"

void ConsoleOutputConsumer::onMessageArrived(const MqttMessage &message) {
    std::cout << "Message recieved!" << std::endl;
    std::cout << "\ttopic: '" << message.topic << "'" << std::endl;
    std::cout << "\tdata: '";

    for (auto word: message.text_fields) {
        std::cout << word << " ";
    }

    std::cout << "'\n" << std::endl;
}
