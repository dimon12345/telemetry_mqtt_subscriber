#pragma once

#include <string>

struct Config {
    static constexpr bool DefaultVerboseFlag = false;
    static constexpr bool DefaultDisablePostgreSql = false;

    bool verbose = DefaultVerboseFlag;
    bool pg_disabled = DefaultDisablePostgreSql;

    // MQTT
    std::string mqtt_host = "mqtt-host";
    unsigned int mqtt_port = 1883;
    std::string mqtt_client_id = "mqtt_subscriber";
    std::string mqtt_topic = "test";
    int mqtt_qos = 1;

    // PostgreSQL
    std::string pg_dbname = "telemetry";
    std::string pg_host = "pg-host";
    std::string pg_user = "lexx";
    std::string pg_password = "xev";
};
