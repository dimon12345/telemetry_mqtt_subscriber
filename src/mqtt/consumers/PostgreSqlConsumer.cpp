#include <iostream>
#include <string>

#include "ConfigOld.h"
#include "PostgreSqlConsumer.h"

PostgreSqlConsumer::PostgreSqlConsumer(const ConfigOld &config) : config_(config) {
    createConnection();
    checkTables();
}

void PostgreSqlConsumer::createConnection() {
    if (config_.verbose) {
        std::cout << "Connect to PostgreSql..." << std::endl;
    }

    std::string connection_string =
            "user=" + config_.pg_user +
            " password=" + config_.pg_password +
            " host=" + config_.pg_host +
            " dbname=" + config_.pg_dbname;
    connection_ = std::make_unique<pqxx::connection>(connection_string);
}

void PostgreSqlConsumer::checkTables() {
    pqxx::work txn(*connection_);
    std::string create_telemetry_table_query =
            "CREATE TABLE IF NOT EXISTS telemetry ("
            "value_id SERIAL PRIMARY KEY, name_id INTEGER NOT NULL, "
            "value FLOAT NOT NULL, timestamp TIMESTAMP NOT NULL)";
    txn.exec(create_telemetry_table_query);
    std::string create_names_table_query =
            "CREATE TABLE IF NOT EXISTS names (name_id SERIAL PRIMARY KEY, "
            "name character varying(255) NOT NULL)";
    txn.exec(create_names_table_query);
    txn.commit();
}

void PostgreSqlConsumer::onMessageArrived(const MqttMessage &message) {
    int sensor_id = getSensorId(message.text_fields[1]);
    std::string value = message.text_fields[2];
    std::string insert_query =
            "INSERT INTO telemetry (name_id, value, timestamp) "
            "VALUES($1, $2, NOW())";

    pqxx::work txn(*connection_);
    txn.exec_params(insert_query, sensor_id, value);
    txn.commit();
}

int PostgreSqlConsumer::getSensorId(const std::string &sensor_name) {
    auto it = names_.find(sensor_name);
    if (it != names_.end()) {
        return it->second;
    }

    pqxx::work txn(*connection_);
    pqxx::result r = txn.exec_params(
            "SELECT * FROM names where name = $1;", sensor_name);
    txn.commit();
    if (r.empty()) {
        return addSensorName(sensor_name);
    }
    return r[0]["name_id"].as<int>();
}

int PostgreSqlConsumer::addSensorName(const std::string &sensor_name) {
    pqxx::work txn(*connection_);
    pqxx::result r = txn.exec_params(
            "INSERT INTO names (name) VALUES ($1) RETURNING name_id;",
            sensor_name);
    txn.commit();
    int name_id = r[0][0].as<int>();
    names_[sensor_name] = name_id;
    return name_id;
}
