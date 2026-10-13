#include "PostgresTelemetryRepository.hpp"

#include <sstream>
#include <iomanip>

namespace {

std::string to_iso8601(std::chrono::system_clock::time_point tp) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    struct tm bt;
#if defined(_WIN32) || defined(_WIN64)
    gmtime_s(&bt, &time);
#else
    gmtime_r(&time, &bt);
#endif
    std::ostringstream oss;
    oss << std::put_time(&bt, "%FT%TZ");
    return oss.str();
}

} // namespace

using namespace Telemetry::Domain;

namespace Telemetry::Infrastructure {

PostgresTelemetryRepository::PostgresTelemetryRepository(const std::string &connection_string) {
    connection_ = std::make_unique<pqxx::connection>(connection_string);
    connection_->prepare("insert_telemetry",
            "INSERT INTO telemetry (sensor_id, value, timestamp) VALUES ($1, $2, $3)");
    connection_->prepare("find_sensor_id", "SELECT id FROM sensors WHERE name = $1");
    connection_->prepare("insert_sensor",
        "INSERT INTO sensors (name) VALUES ($1) RETURNING id;");

}

void PostgresTelemetryRepository::save(const TelemetryMessage &message) {
    int sensor_id = -1;
    const auto &name = message.name;
    auto iter = sensor_ids_.find(name);
    if (iter == sensor_ids_.end()) {
        sensor_id = getSensorId(name);
        sensor_ids_[name] = sensor_id;
    } else {
        sensor_id = iter->second;
    }

    pqxx::work txn(*connection_);
    txn.exec_prepared(
            "insert_telemetry",
            sensor_id,
            message.value,
            to_iso8601(message.timestamp));
    txn.commit();
}

int PostgresTelemetryRepository::getSensorId(const std::string &name) {
    pqxx::nontransaction txn(*connection_);
    pqxx::result r = txn.exec_prepared("find_sensor_id", name);
    if (!r.empty()) {
        return r[0]["id"].as<int64_t>();
    }

    r = txn.exec_prepared("insert_sensor", name);
    txn.commit();
    return r[0][0].as<int64_t>();
}

} // namespace Telemetry::Infrastructure
