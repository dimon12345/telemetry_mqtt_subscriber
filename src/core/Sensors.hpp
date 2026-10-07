#pragma once

#include <optional>
#include <string>

namespace Telemetry::Domain {

struct SensorValue {
    std::string name;
    float value;
};

class ISensorValueService {
public:
    virtual void save(const SensorValue& value) = 0;
};

struct SensorMeasurement {
    int sensor_id;
    float value;
    std::chrono::system_clock::time_point timestamp;
};

class ISensorRepository {
public:
    virtual std::optional<int> getIdByName(const std::string &name);
    virtual int addName(const std::string &name);

    virtual void addMeasurement(const SensorMeasurement &measurement);
};

} // namespace Telemetry::Domain
