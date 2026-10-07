#pragma once

#include <pqxx/pqxx>

#include "core/Sensors.hpp"

using namespace Telemetry::Domain;

namespace Telemetry::Infrastructure {

class PqxxSensorRepository : public ISensorRepository {
public:
    PqxxSensorRepository(std::shared_ptr<pqxx::connection> connection)
            : connection_(connection) {
        connection_->prepare("find_sensor_id", "SELECT id FROM sensors WHERE name = $1");
        connection_->prepare("insert_sensor",
            "INSERT INTO sensors (name) VALUES ($1) RETURNING id;");
        connection_->prepare("insert_measurement",
            "INSERT INTO measurements (sensor_id, value, timestamp) VALUES ($1, $2, $3)");
    }

    std::optional<int> getSensorIdByName(const std::string &name) override {
        pqxx::work txn(*connection_);
        pqxx::result r = txn.exec_prepared("find_sensor_id", name);
        if (r.empty()) {
            return std::nullopt;
        }
        return r[0]["id"].as<int64_t>();
    }

    int addSensor(const std::string &name) override {
        pqxx::work txn(*connection_);
        pqxx::result r = txn.exec_prepared("insert_sensor", name);
        txn.commit();
        return r[0][0].as<int64_t>();
    }

    void addMeasurement(const SensorMeasurement &measurement) override {
        pqxx::work txn(*connection_);
        txn.exec_prepared("insert_measurement", measurement.sensor_id, measurement.value, measurement.timestamp);
        txn.commit();
    }
private:
    std::shared_ptr<pqxx::connection> connection_;
};

} // namespace Telemetry::Infrastructure
