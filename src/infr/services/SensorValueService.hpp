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
            auto sensor_id_value = repository_->getSensorIdByName(value.name);
            if (sensor_id_value.has_value()) {
                sensor_id = sensor_id_value.value();
            } else {
                sensor_id = repository_->addSensor(value.name);
                sensor_ids_[value.name] = sensor_id;
            }
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
