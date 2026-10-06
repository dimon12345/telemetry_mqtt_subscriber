#include <iostream>
#include "app/ArgParseConfig.hpp"

int main(int argc, char* argv[]) {
    ArgParseConfig config;

    if (!config.loadFromArgs(argc, argv)) {
        return 1;
    }

    const auto& pg = config.getPostgres();
    std::cout << "Postgres: " << pg.toConnectionString() << "\n";
    const auto & mqtt = config.getMqttConfig();
    std::cout << "MQTT: " << mqtt.toConnectionString() << "\n";

    return 0;
}
