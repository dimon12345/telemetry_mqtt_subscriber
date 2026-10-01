#include <iostream>

#include "app/ArgParseConfig.h"
#include "app/SignalManager.h"
#include "mqtt/MqttSubscriber.h"
#include "mqtt/consumers/ConsoleOutputConsumer.h"
#include "mqtt/consumers/PostgreSqlConsumer.h"

int main(int argc, char **argv) {
    try {
        ArgParseConfig config(argc, argv);

        std::cout << "Connect to MQTT server..." << std::endl;
        MqttSubscriber mqtt_subscriber(config);

        std::shared_ptr <MqttMessageConsumer> console_output_consumer;
        if (config.verbose) {
            console_output_consumer =
                    std::make_shared<ConsoleOutputConsumer>();
            mqtt_subscriber.subscribe(console_output_consumer);
        }

        std::shared_ptr <MqttMessageConsumer> pg_consumer;
        if (!config.pg_disabled) {
            pg_consumer = std::make_shared<PostgreSqlConsumer>(config);
            mqtt_subscriber.subscribe(pg_consumer);
        }

        mqtt_subscriber.start();

        SignalManager::instance().waitForSignal();

        mqtt_subscriber.stop();
        return 0;
    }
    catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
