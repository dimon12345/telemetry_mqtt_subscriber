#pragma once

#include <string>

struct MqttConfig {
    std::string server_uri = "tcp://mqtt-host:1883";
    std::string client_id = "mqtt_subscriber";
    std::string user = "postgres";
    std::string password = "postgres";
    std::string topic = "house/#";
};

struct PostgresConfig {
    std::string host = "pg-host";
    int port = 5432;
    std::string user = "postgres";
    std::string password = "postgres";
    std::string dbname = "telemetry";

    std::string toConnectionString() const {
        return "host=" + host +
               " port=" + std::to_string(port) +
               " user=" + user +
               " password=" + password +
               " dbname=" + dbname;
    }
};

class Config {
public:
    const MqttConfig &getMqttConfig() const {return mqtt_config_;}
    const PostgresConfig &getPostgres() const {return pg_config_;}

protected:
    MqttConfig mqtt_config_;
    PostgresConfig pg_config_;
};
