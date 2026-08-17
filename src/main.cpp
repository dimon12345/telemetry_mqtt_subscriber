#include <iostream>

#include "app/Config.h"
#include "mqtt/MqttSubscriber.h"
#include "mqtt/ConsoleOutputConsumer.h"
#include "mqtt/PostgreSqlConsumer.h"

int main(int argc, char **argv) {
    try {
        Config config(argc, argv);
        MqttSubscriber mqtt_subscriber(config);

        std::shared_ptr<MqttMessageConsumer> console_output_consumer;
        if (config.verbose()) {
            console_output_consumer =
                    std::make_shared<ConsoleOutputConsumer>();
            mqtt_subscriber.subscribe(console_output_consumer);
        }

        std::shared_ptr<MqttMessageConsumer> consumer =
                std::make_shared<PostgreSqlConsumer>(config);
        mqtt_subscriber.subscribe(consumer);

        return mqtt_subscriber.run();
    }
    catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
