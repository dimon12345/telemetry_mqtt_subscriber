#include <benchmark/benchmark.h>
#include <mqtt/async_client.h>
#include <pqxx/pqxx>
#include <string>
#include <chrono>
#include <thread>
#include <memory>

#include "ConfigOld.h"
#include "mqtt/MqttSubscriber.h"
#include "mqtt/consumers/PostgreSqlConsumer.h"


const std::string BROKER_ADDRESS = "tcp://mqtt-host:1883";
const int MQTT_QOS = 1;
const std::string TEST_TOPIC = "benchmark_test";
const std::string SUBSCRIBER_CLIENT_ID = "benchmark_subscriber";

const std::string DB_NAME = "telemetry_test";
const std::string DB_CONN_STR = "dbname=telemetry_test user=lexx password=xev host=mqtt-host port=5432";

long long get_db_row_count(pqxx::connection& conn) {
    pqxx::work tx(conn);
    pqxx::result res = tx.exec("SELECT COUNT(*) FROM telemetry;");
    tx.commit();
    return res[0][0].as<long long>();
}

class MqttSingleConnFixture : public benchmark::Fixture {
public:
    std::unique_ptr<ConfigOld> config;
    std::unique_ptr<MqttSubscriber> server;
    std::shared_ptr<MqttMessageConsumer> pg_consumer;
    std::unique_ptr<pqxx::connection> db_conn;
    std::unique_ptr<mqtt::async_client> client;

    void SetUp(const ::benchmark::State& state) override {
        if (!config) {
            config = std::make_unique<ConfigOld>();
            config->pg_dbname = DB_NAME;
            config->mqtt_topic = TEST_TOPIC;
            config->mqtt_qos = MQTT_QOS;
            config->mqtt_client_id = SUBSCRIBER_CLIENT_ID;

            server = std::make_unique<MqttSubscriber>(*config);
            pg_consumer = std::make_shared<PostgreSqlConsumer>(*config);
            server->subscribe(pg_consumer);
            server->start();

            db_conn = std::make_unique<pqxx::connection>(DB_CONN_STR);

            client = std::make_unique<mqtt::async_client>(BROKER_ADDRESS, "benchmark_publisher");

            auto connOpts = mqtt::connect_options_builder()
                    .clean_session(true)
                    .user_name("lexx")
                    .password("xev")
                    .finalize();

            client->connect(connOpts)->wait();
        }

        pqxx::work tx(*db_conn);
        tx.exec("TRUNCATE TABLE telemetry;");
        tx.commit();
    }

    void TearDown(const ::benchmark::State& state) override {
        server->stop();
    }

    ~MqttSingleConnFixture() {
        if (client && client->is_connected()) {
            try {
                client->disconnect()->wait();
            } catch (...) {
            }
        }
    }
};


BENCHMARK_DEFINE_F(MqttSingleConnFixture, SendViaSingleConnection)(benchmark::State& state) {
    const int num_messages = state.range(0);

    for (auto _ : state) {
        long long start_count = get_db_row_count(*db_conn);
        long long expected_count = start_count + num_messages;

        for (int i = 0; i < num_messages; ++i) {
            std::string payload = "sensor benchmark 0 0";
            client->publish(TEST_TOPIC, payload, MQTT_QOS, false);
        }
        while (true) {
            long long current_count = get_db_row_count(*db_conn);
            if (current_count >= expected_count) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}

BENCHMARK_REGISTER_F(MqttSingleConnFixture, SendViaSingleConnection)
    ->Arg(10)
    ->Unit(benchmark::kMillisecond)
    ->Iterations(5);

BENCHMARK_MAIN();
