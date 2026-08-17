#pragma once

#include <cstdint>
#include <map>
#include <memory>

#include <pqxx/pqxx>

#include "core/TelemetryMessage.hpp"

namespace Telemetry::Infrastructure {

class PostgresTelemetryRepository
    : public Telemetry::Domain::ITelemetryRepository {
public:
    PostgresTelemetryRepository(const std::string &connection_string);

    void save(const Telemetry::Domain::TelemetryMessage &message) override;

private:
    const std::string connection_string_;
    std::unique_ptr<pqxx::connection> connection_;
    std::unordered_map<std::string, std::int64_t> sensor_ids_;

    void connect();
    std::int64_t getSensorId(const std::string &name);
};

} // namespace Telemetry::Infrastructure
