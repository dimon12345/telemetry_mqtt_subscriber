#pragma once

#include <string>

namespace Telemetry::Infrastructure {

struct MqttConfig {
    std::string server_uri = "tcp://mqtt-host:1883";
    std::string client_id = "mqtt_subscriber";
    std::string user = "postgres";
    std::string password = "postgres";
    std::string topic = "house/#";
};

} // namespace Telemetry::Infrastructure
