#include <benchmark/benchmark.h>

#include <string>
#include <chrono>
#include <thread>
#include <memory>

#include <mqtt/async_client.h>
#include <pqxx/pqxx>

#include "Config.hpp"
#include "core/TelemetryMessage.hpp"
#include "infr/repos/PostgresTelemetryRepository.hpp"
#include "infr/MqttListener.hpp"
#include "infr/MqttProcessor.hpp"

using namespace Telemetry::Infrastructure;

namespace {
    std::string get_env_var(const std::string& key) {
        if (const char* val = std::getenv(key.c_str())) {
            return val;
        }
        return "";
    }

    long long get_db_row_count(pqxx::connection& conn) {
        pqxx::nontransaction tx(conn);
        pqxx::result res = tx.exec("SELECT COUNT(*) FROM telemetry;");
        return res[0][0].as<long long>();
    }

    void clear_db(pqxx::connection& conn) {
        pqxx::work tx(conn);
        tx.exec("TRUNCATE TABLE telemetry;");
        tx.commit();
    }
} // namespace

class BenchConfig : public Config {
public:
    BenchConfig() {
        mqtt_config_.topic = "house/benchmark_test";
        mqtt_config_.client_id = "benchmark_subscriber";
        mqtt_config_.server_uri = "tcp://" + get_env_var("MQTT_HOST") + ":1883";
        mqtt_config_.user = get_env_var("MQTT_USER");
        mqtt_config_.password = get_env_var("MQTT_PASSWORD");

        pg_config_.dbname = "telemetry_test";
        pg_config_.host = get_env_var("POSTGRES_HOST");
        pg_config_.user = get_env_var("POSTGRES_USER");
        pg_config_.password = get_env_var("POSTGRES_PASSWORD");
    }
};

class MqttSingleConnFixture : public benchmark::Fixture {
public:
    std::shared_ptr<pqxx::connection> test_postgres_connection_;
    std::shared_ptr<mqtt::async_client> mqtt_client_;
    BenchConfig config_;

    void SetUp(const ::benchmark::State& state) override {
        auto &postgres_config = config_.getPostgresConfig();
        auto db_connection_string = postgres_config.toConnectionString();

        std::shared_ptr<Telemetry::Domain::ITelemetryRepository> repository =
                std::make_shared<Telemetry::Infrastructure::PostgresTelemetryRepository>(db_connection_string);

        processor_ = std::make_shared<Telemetry::Infrastructure::MqttProcessor>(std::move(repository));
        processor_->start();

        auto &mqtt_config = config_.getMqttConfig();
        listener_ = std::make_shared<MqttListener>(processor_, mqtt_config);
        listener_->connect();

        mqtt_client_ = std::make_shared<mqtt::async_client>(mqtt_config.server_uri, "benchmark_publisher");
        mqtt::connect_options conn_opts;
        conn_opts.set_keep_alive_interval(20);
        conn_opts.set_clean_session(true);
        conn_opts.set_automatic_reconnect(true);

        conn_opts.set_user_name(get_env_var("MQTT_USER"));
        conn_opts.set_password(get_env_var("MQTT_PASSWORD"));
        mqtt_client_->connect(conn_opts)->wait();


        test_postgres_connection_ = std::make_shared<pqxx::connection>(db_connection_string);
        clear_db(*test_postgres_connection_);
    }

    void TearDown(const ::benchmark::State& state) override {
        mqtt_client_->disconnect()->wait();
        listener_->disconnect();
        processor_->stop();
    }

private:
    std::shared_ptr<Telemetry::Infrastructure::MqttListener> listener_;
    std::shared_ptr<Telemetry::Infrastructure::MqttProcessor> processor_;
};

BENCHMARK_DEFINE_F(MqttSingleConnFixture, SendViaSingleConnection)(benchmark::State& state) {
    const int num_messages = state.range(0);
    const auto &topic = config_.getMqttConfig().topic;

    for (auto _ : state) {
        long long start_count = get_db_row_count(*test_postgres_connection_);
        long long expected_count = start_count + num_messages;

        for (int i = 0; i < num_messages; ++i) {
            std::string payload = "{\"temperature\":0}";
            constexpr int QOS = 1;
            mqtt_client_->publish(topic, payload, QOS, false);
        }
        while (true) {
            long long current_count = get_db_row_count(*test_postgres_connection_);
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
    ->Iterations(100);

BENCHMARK_MAIN();
