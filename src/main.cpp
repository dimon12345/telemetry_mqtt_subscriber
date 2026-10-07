#include <iostream>
#include <memory>

#include <mqtt/async_client.h>

#include "app/ArgParseConfig.hpp"
#include "app/SignalManager.hpp"
#include "core/Sensors.hpp"
#include "infr/database/PqxxSensorRepository.hpp"
#include "infr/services/MqttTelemetryService.hpp"
#include "infr/services/SensorValueService.hpp"


using namespace Telemetry::Domain;
using namespace Telemetry::Infrastructure;

int main(int argc, char* argv[]) {
    ArgParseConfig config;

    if (!config.loadFromArgs(argc, argv)) {
        return 1;
    }

    try {
        const auto &postgres_config = config.getPostgres();
        auto connection_string = postgres_config.toConnectionString();
        auto connection = std::make_unique<pqxx::connection>(connection_string);
        auto sensor_repository = std::make_shared<PqxxSensorRepository>(std::move(connection));
        auto sensor_value_service = std::make_shared<SensorValueService>(std::move(sensor_repository));
        auto mqtt_telemetry_service = std::make_shared<MqttTelemetryService>(std::move(sensor_value_service));

        const auto &mqtt_config = config.getMqttConfig();

        mqtt_telemetry_service->start();
        mqtt::async_client client(mqtt_config.server_uri, mqtt_config.client_id);
        client.set_callback(*mqtt_telemetry_service);

        mqtt::connect_options conn_opts;
        conn_opts.set_keep_alive_interval(20);
        conn_opts.set_clean_session(true);
        conn_opts.set_automatic_reconnect(true);
        conn_opts.set_user_name(mqtt_config.user);
        conn_opts.set_password(mqtt_config.password);

        client.connect(conn_opts)->wait();
        constexpr int QOS = 1;
        client.subscribe(mqtt_config.topic, QOS)->wait();

        std::cout << "Press Ctrl-C" << std::endl;
        SignalManager::instance().waitForSignal();

        client.disconnect()->wait();
        mqtt_telemetry_service->stop();
        return 0;
    } catch (const mqtt::exception& exc) {
        std::cerr << "\n[MQTT Client Error]: " << exc.what() << std::endl;
        return 1;
    } catch (const std::exception& exc) {
        std::cerr << "\n[System Error]: " << exc.what() << std::endl;
        return 1;
    }
}
