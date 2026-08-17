#include <ranges>

#include "mqtt/async_client.h"

#include "app/CtrlCHandler.h"
#include "app/Config.h"
#include "MqttMessageConsumer.h"
#include "MqttSubscriber.h"

int MqttSubscriber::run() {
    try {
        std::string server_address = "tcp://" + config_.mqtt_host() + ":" +
                std::to_string(config_.mqtt_port());
        mqtt::async_client client(server_address, config_.mqtt_client_id());
        client.set_callback(*this);

        mqtt::connect_options connOpts;
        connOpts.set_keep_alive_interval(20);
        connOpts.set_clean_session(true);
        connOpts.set_user_name("lexx");
        connOpts.set_password("xev");

        std::cout << "Соединение с брокером [" << server_address <<
                     "] ..." << std::endl;
        client.connect(connOpts)->wait();

        std::cout << "Подписка на топик: " << config_.mqtt_topic() <<
                     std::endl;
        client.subscribe(config_.mqtt_topic(), config_.mqtt_qos())->wait();
        std::cout << "Подписка активна. Ожидание сообщений (нажмите Ctrl-C "
                     "для выхода)..." << std::endl;

        CtrlCHandler::getInstance().wait();

        std::cout << "Отписка от топика..." << std::endl;
        client.unsubscribe(config_.mqtt_topic())->wait();
        client.disconnect()->wait();
        std::cout << "Завершено." << std::endl;

    } catch (const mqtt::exception& exc) {
        std::string reason = exc.get_message();
        std::string message = std::string(exc.what()) + " [" + reason + "]";
        throw std::runtime_error(message);
    }
    return 0;
}

void MqttSubscriber::subscribe(std::weak_ptr<MqttMessageConsumer> subscriber) {
    std::lock_guard<std::mutex> lock(subscribers_mutex_);
    subscribers_.push_back(subscriber);
}

void MqttSubscriber::message_arrived(mqtt::const_message_ptr msg) {
    MqttMessage message;
    message.topic = msg->get_topic();
    auto fields = msg->to_string() | std::views::split(' ');

    for (auto&& word : fields) {
        message.text_fields.emplace_back(word.begin(), word.end());
    }

    std::lock_guard<std::mutex> lock(subscribers_mutex_);
    for (auto weak_subscriber : subscribers_ ) {
        if (auto subscriber = weak_subscriber.lock()) {
            subscriber->onMessageArrived(message);
        }
    }
}
