#pragma once

#include <chrono>
#include <stdexcept>
#include <string>

namespace Telemetry::Domain {

struct TelemetryMessage {
    std::string name;
    float value;
    std::chrono::system_clock::time_point timestamp;
};

class ITelemetryRepository {
public:
    virtual void save(const TelemetryMessage &message) = 0;
};

class TelemetryConnectionError : public std::runtime_error {
public:
    explicit TelemetryConnectionError(const std::string &message)
        : std::runtime_error(message) {}
};

} // namespace Telemetry::Domain
