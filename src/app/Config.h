#pragma once

#include <string>

class Config {
public:
    static constexpr bool DefaultVerboseFlag = false;
    inline static constexpr std::string AppName = "Event monitor";
    inline static constexpr std::string DefaultConfigFilename = "config.json";
    inline static constexpr std::string DefaultMqttHostname = "mqtt-host";
    static constexpr unsigned int DefaultMqttPort = 1883;
    inline static constexpr std::string DefaultMqttClientId = "mqtt_subscriber";
    inline static constexpr std::string DefaultMqttTopic = "test";
    static constexpr int DefaultMqttQos = 0;
    inline static constexpr std::string DefaultPostgreSqlHostname = "pg-host";
    inline static constexpr std::string DefaultPostgreSqlUser = "lexx";
    inline static constexpr std::string DefaultPostgreSqlPassword = "xev";
    inline static constexpr std::string DefaultPostgreSqlDbname = "telemetry";

    Config(int argc, char **argv);

    bool verbose() const {
        return verbose_;
    }

    const std::string &mqtt_host() const {
        return mqtt_host_;
    }

    int mqtt_port() const {
        return mqtt_port_;
    }

    const std::string &mqtt_client_id() const {
        return mqtt_client_id_;
    }

    const std::string &mqtt_topic() const {
        return mqtt_topic_;
    }

    int mqtt_qos() const {
        return mqtt_qos_;
    }

    const std::string &pg_host() const {
        return pg_host_;
    }

    const std::string &pg_user() const {
        return pg_user_;
    }

    const std::string &pg_password() const {
        return pg_password_;
    }

    const std::string &pg_dbname() const {
        return pg_dbname_;
    }

private:
    bool verbose_ = DefaultVerboseFlag;

    std::string mqtt_host_ = DefaultMqttHostname;
    unsigned int mqtt_port_ = DefaultMqttPort;
    std::string mqtt_client_id_ = DefaultMqttClientId;
    std::string mqtt_topic_ = DefaultMqttTopic;
    int mqtt_qos_ = DefaultMqttQos;

    std::string pg_user_ = DefaultPostgreSqlUser;
    std::string pg_password_ = DefaultPostgreSqlPassword;
    std::string pg_dbname_ = DefaultPostgreSqlDbname;
    std::string pg_host_ = DefaultPostgreSqlHostname;

    void validate();
};
