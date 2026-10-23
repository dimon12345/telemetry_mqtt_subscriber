#include "MqttListener.hpp"

#include <mqtt/async_client.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
    std::string getSensorName(std::string_view topic) {
        size_t last_slash = topic.find_last_of('/');
        if (last_slash == std::string_view::npos) {
            return std::string(topic);
        }
        std::string_view state = topic.substr(last_slash + 1);
        if (state != "state") {
            return std::string(state);
        }

        size_t name_slash = topic.substr(0, last_slash).find_last_of('/');
        if (name_slash == std::string_view::npos) {
            return std::string(state);
        }

        std::string_view name = topic.substr(name_slash + 1, last_slash - name_slash -1);
        return std::string(name);
    }
}

using namespace Telemetry::Domain;

namespace Telemetry::Infrastructure {

MqttListener::~MqttListener() {
    disconnect();
}

void MqttListener::connect() {
    mqtt_client_ = std::make_unique<mqtt::async_client>(config_.server_uri, config_.client_id);
    mqtt_client_->set_callback(*this);

    mqtt::connect_options conn_opts;
    conn_opts.set_keep_alive_interval(20);
    conn_opts.set_clean_session(true);
    conn_opts.set_automatic_reconnect(true);
    conn_opts.set_user_name(config_.user);
    conn_opts.set_password(config_.password);

    mqtt_client_->connect(conn_opts)->wait();
    constexpr int QOS = 1;
    mqtt_client_->subscribe(config_.topic, QOS)->wait();
}

void MqttListener::disconnect() {
    if (mqtt_client_ && mqtt_client_->is_connected()) {
        mqtt_client_->unsubscribe(config_.topic)->wait();
        mqtt_client_->disconnect()->wait();
    }
}

void MqttListener::message_arrived(mqtt::const_message_ptr msg) {
    std::string name = getSensorName(msg->get_topic());
    try {
        float value = parseTemperature(msg);
        auto now = std::chrono::system_clock::now();
        processor_->pushMessage(TelemetryMessage{name, value, now});
    } catch (const std::invalid_argument &e) {
        std::cerr << "Parse error. Skip mqtt message '" << msg->to_string() <<
                "' and topic '" << msg->get_topic() << "' and name '" << name <<
                "'" << std::endl;
    }
}

float MqttListener::parseTemperature(mqtt::const_message_ptr msg) {
    auto payload = msg->get_payload();

    try {
        json data = json::parse(payload.begin(), payload.end());
        auto it = data.find("temperature");
        if (it == data.end()) {
            throw std::invalid_argument("temperature field not found");
        }

        if (!it->is_number()) {
            throw std::invalid_argument("temperature field is not number");
        }

        return it->get<double>();
    } catch (const json::parse_error& e) {
        throw std::invalid_argument(std::string("Parsing error: ") + e.what());
    }
}

} // namespace Telemetry::Infrastructure
