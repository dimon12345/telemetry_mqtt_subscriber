#include <iostream>

#include "ConsoleOutputConsumer.h"

void ConsoleOutputConsumer::onMessageArrived(const MqttMessage &message) {
    std::cout << "Получено сообщение!" << std::endl;
    std::cout << "\tтопик: '" << message.topic << "'" << std::endl;
    std::cout << "\tданные: '";

    for (auto word: message.text_fields) {
        std::cout << word << " ";
    }

    std::cout << "'\n" << std::endl;
}
