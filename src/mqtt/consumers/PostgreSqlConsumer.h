#pragma once

#include <map>
#include <memory>
#include <string>

#include <pqxx/pqxx>

#include "../MqttMessageConsumer.h"

class PostgreSqlConsumer : public MqttMessageConsumer {
public:
    PostgreSqlConsumer(const class ConfigOld &config);
    void onMessageArrived(const MqttMessage &message);

private:
    const class ConfigOld &config_;
    std::unique_ptr<pqxx::connection> connection_;
    std::map<std::string, int> names_;

    void createConnection();
    void checkTables();
    int getSensorId(const std::string &sensor_name);
    int addSensorName(const std::string &sensor_name);
};
