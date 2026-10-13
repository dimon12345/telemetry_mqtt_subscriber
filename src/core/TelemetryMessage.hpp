#pragma once

#include <chrono>
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

} // namespace Telemetry::Domain
