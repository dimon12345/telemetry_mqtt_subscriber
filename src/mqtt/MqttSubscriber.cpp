#include <sstream>

#include "mqtt/async_client.h"

#include "Config.h"
#include "MqttMessageConsumer.h"
#include "MqttSubscriber.h"

namespace {
    std::vector<std::string> mapStringToFields(const std::string &raw_fields) {
        std::istringstream stream(raw_fields);
        std::vector<std::string> fields;
        std::string field;
        while(stream) {
            stream >> field;
            fields.push_back(field);
        }
        return fields;
    }
} // namespace

void MqttSubscriber::start() {
    if (client_) {
        return;
    }

    try {
        std::string server_address = "tcp://" + config_.mqtt_host + ":" +
                                     std::to_string(config_.mqtt_port);
        client_ = std::make_unique<mqtt::async_client>(server_address, config_.mqtt_client_id);
        client_->set_callback(*this);

        mqtt::connect_options connOpts;
        connOpts.set_keep_alive_interval(20);
        connOpts.set_clean_session(true);
        connOpts.set_user_name("lexx");
        connOpts.set_password("xev");

        std::cout << "Connect to MQTT broker [" << server_address <<
                  "] ..." << std::endl;
        client_->connect(connOpts)->wait();

        std::cout << "Subscribe to topic: " << config_.mqtt_topic <<
                  std::endl;
        client_->subscribe(config_.mqtt_topic, config_.mqtt_qos)->wait();
        std::cout << "Subscription active. Wait for messages (press Ctrl-C "
                     "to exit)..." << std::endl;

        client_->subscribe("test", 0);

    } catch (const mqtt::exception &exc) {
        std::string reason = exc.get_message();
        std::string message = std::string(exc.what()) + " [" + reason + "]";
        throw std::runtime_error(message);
    }
}

void MqttSubscriber::stop() {
    try {

        std::cout << "Unsubscribe from topic..." << std::endl;
        client_->unsubscribe(config_.mqtt_topic)->wait();
        client_->disconnect()->wait();
        std::cout << "Finished." << std::endl;
        client_.reset();

    } catch (const mqtt::exception& exc) {
        std::string reason = exc.get_message();
        std::string message = std::string(exc.what()) + " [" + reason + "]";
        throw std::runtime_error(message);
    }
}

void MqttSubscriber::subscribe(std::weak_ptr<MqttMessageConsumer> subscriber) {
    std::lock_guard<std::mutex> lock(subscribers_mutex_);
    subscribers_.push_back(subscriber);
}

void MqttSubscriber::message_arrived(mqtt::const_message_ptr msg) {
    MqttMessage message;
    message.topic = msg->get_topic();
    message.text_fields = mapStringToFields(msg->to_string());

    std::lock_guard<std::mutex> lock(subscribers_mutex_);
    for (auto weak_subscriber : subscribers_ ) {
        if (auto subscriber = weak_subscriber.lock()) {
            subscriber->onMessageArrived(message);
        }
    }
}
