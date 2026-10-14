#include <mqtt/async_client.h>

#include "MqttListener.hpp"

namespace {
    std::string getSensorName(std::string_view topic) {
        size_t last_slash = topic.find_last_of('/');
        if (last_slash != std::string_view::npos && last_slash < topic.length() - 1) {
            return std::string(topic.substr(last_slash + 1));
        }
        return std::string(topic);
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
        float value = std::stof(msg->to_string());
        auto now = std::chrono::system_clock::now();
        processor_->pushMessage(TelemetryMessage{name, value, now});
    } catch (const std::invalid_argument &e) {
        std::cerr << "Invalid argument. Skip mqtt message '" << msg->to_string() <<
                "' and topic '" << msg->get_topic() << "'" << std::endl;
        return;
    }
}

} // namespace Telemetry::Infrastructure
