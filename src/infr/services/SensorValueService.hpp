#pragma once

#include <memory>

#include "common/SystemClock.hpp"
#include "core/Sensors.hpp"

using namespace Telemetry::Common;

namespace Telemetry::Infrastructure {

class SensorValueService : public Telemetry::Domain::ISensorValueService {
public:
    SensorValueService(std::shared_ptr<Telemetry::Domain::ISensorRepository> repository)
            : repository_(std::move(repository)) {
    }

    void save(const SensorValue& value) override {
        int sensor_id = -1;
        auto iter = sensor_ids_.find(value.name);
        if (iter == sensor_ids_.end()) {
            sensor_id = repository_->addSensor(value.name);
        } else {
            sensor_id = iter->second;
        }

        auto current_time = std::chrono::system_clock::now();
        auto timestamp = to_iso8601(current_time);

        repository_->addMeasurement(SensorMeasurement{sensor_id, value.value, timestamp});
    }

private:
    std::shared_ptr<Telemetry::Domain::ISensorRepository> repository_;
    std::unordered_map<std::string, int> sensor_ids_;
};

} // namespace Telemetry::Infrastructure
