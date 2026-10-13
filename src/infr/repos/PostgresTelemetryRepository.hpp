#pragma once

#include <map>
#include <memory>

#include <pqxx/pqxx>

#include "core/TelemetryMessage.hpp"

namespace Telemetry::Infrastructure {

class PostgresTelemetryRepository : public Telemetry::Domain::ITelemetryRepository {
public:
    PostgresTelemetryRepository(const std::string &connection_string);

    void save(const Telemetry::Domain::TelemetryMessage &message) override;
private:
    std::unique_ptr<pqxx::connection> connection_;
    std::unordered_map<std::string, int> sensor_ids_;
    int getSensorId(const std::string &name);
};

} // namespace Telemetry::Infrastructure
