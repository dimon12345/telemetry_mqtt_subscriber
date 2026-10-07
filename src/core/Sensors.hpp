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
    std::string timestamp; // ISO 8601
};

class ISensorRepository {
public:
    virtual std::optional<int> getSensorIdByName(const std::string &name) = 0;
    virtual int addSensor(const std::string &name) = 0;

    virtual void addMeasurement(const SensorMeasurement &measurement) = 0;
};

} // namespace Telemetry::Domain
