#include <iostream>
#include <memory>

//#include <mqtt/callback.h>
#include <mqtt/async_client.h>

#include "app/ArgParseConfig.hpp"
#include "app/SignalManager.hpp"
#include "core/Sensors.hpp"

#include "infr/services/MqttTelemetryService.hpp"

using namespace Telemetry::Domain;
using namespace Telemetry::Infrastructure;

namespace {

class MockSensorValueService : public ISensorValueService {
public:
    void save(const SensorValue& value) override {
        std::cout << "-----> save sensor value:" << std::endl <<
                     "sensor_name: " << value.name << std::endl <<
                     "value: " << value.value << std::endl;
    }
};

} // namespace

int main(int argc, char* argv[]) {
    ArgParseConfig config;

    if (!config.loadFromArgs(argc, argv)) {
        return 1;
    }

    try {
        std::cout << "Initializing system modules..." << std::endl;
        auto sensor_value_service = std::make_shared<MockSensorValueService>();
        auto mqtt_telemetry_service = std::make_shared<MqttTelemetryService>(sensor_value_service);

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
