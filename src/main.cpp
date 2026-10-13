#include <iostream>

#include "app/SignalManager.hpp"
#include "app/ArgParseConfig.hpp"
#include "infr/MqttListener.hpp"
#include "infr/MqttProcessor.hpp"
#include "infr/repos/PostgresTelemetryRepository.hpp"

using namespace Telemetry::Infrastructure;

int main(int argc, char* argv[]) {
    try {
        ArgParseConfig config(argc, argv);
        auto connection_string =
                config.getPostgresConfig().toConnectionString();
        auto repository = std::make_shared<PostgresTelemetryRepository>(connection_string);
        auto processor = std::make_shared<MqttProcessor>(repository);
        processor->start();

        const auto &mqtt_config = config.getMqttConfig();
        auto listener = std::make_shared<MqttListener>(processor, mqtt_config);
        listener->connect();

        std::cout << "Press Ctrl-C" << std::endl;
        SignalManager::instance().waitForSignal();

        listener->disconnect();
        processor->stop();
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
