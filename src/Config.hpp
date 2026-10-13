#pragma once

#include "infr/configs/MqttConfig.hpp"
#include "infr/configs/PostgresConfig.hpp"

class Config {
public:
    const Telemetry::Infrastructure::MqttConfig &getMqttConfig() const {return mqtt_config_;}
    const Telemetry::Infrastructure::PostgresConfig &getPostgresConfig() const {return pg_config_;}

protected:
    Telemetry::Infrastructure::MqttConfig mqtt_config_;
    Telemetry::Infrastructure::PostgresConfig pg_config_;
};
