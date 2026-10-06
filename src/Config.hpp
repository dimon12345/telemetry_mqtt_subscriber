#pragma once

#include <string>

struct PostgresConfig {
    std::string host = "pg-host";
    int port = 5432;
    std::string user = "lexx";
    std::string password = "xev";
    std::string dbname = "telemetry";

    std::string toConnectionString() const {
        return "host=" + host +
               " port=" + std::to_string(port) +
               " user=" + user +
               " password=" + password +
               " dbname=" + dbname;
    }
};

struct MqttConfig {
    std::string host = "mqtt-host";
    int port = 1883;
    std::string client_id;
    std::string user = "lexx";
    std::string password = "xev";
    std::string topic;

    std::string toConnectionString() const {
        return "mqtt://" + user + ":" + password +
               "@" + host + ":" + std::to_string(port) +
               "/" + topic;
    }
};

class Config {
public:
    const PostgresConfig &getPostgres() const {return pg_config_;}
    const MqttConfig &getMqttConfig() const {return mqtt_config_;}

protected:
    PostgresConfig pg_config_;
    MqttConfig mqtt_config_;
};
