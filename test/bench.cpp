#include <benchmark/benchmark.h>

#include <string>
#include <chrono>
#include <thread>
#include <memory>

#include <mqtt/async_client.h>
#include <pqxx/pqxx>

#include "Config.hpp"
#include "app/ArgParseConfig.hpp"
#include "core/TelemetryMessage.hpp"
#include "infr/repos/PostgresTelemetryRepository.hpp"
#include "infr/MqttListener.hpp"
#include "infr/MqttProcessor.hpp"

using namespace Telemetry::Infrastructure;

namespace {
    static const int bench_argc = 1;
    static const char *bench_argv[] = {"./bench", NULL};
    std::string get_env_var(const std::string& key, const std::string &default_value) {
        if (const char* val = std::getenv(key.c_str())) {
            return val;
        }
        return default_value;
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

class BenchConfig : public ArgParseConfig {
public:
    BenchConfig() : ArgParseConfig(bench_argc, bench_argv) {
        mqtt_config_.topic = "house/benchmark_test/state";
        mqtt_config_.client_id = "benchmark_subscriber";
        pg_config_.dbname = "telemetry_test";

        std::string mqtt_host = get_env_var("MQTT_HOST", "");
        if (!mqtt_host.empty()) {
            mqtt_config_.server_uri = "tcp://" + mqtt_host + ":1883";
        }

        mqtt_config_.user = get_env_var("MQTT_USER", mqtt_config_.user);
        assert(!mqtt_config_.user.empty());

        mqtt_config_.password = get_env_var("MQTT_PASSWORD", mqtt_config_.password);
        assert(!mqtt_config_.password.empty());

        pg_config_.host = get_env_var("POSTGRES_HOST", pg_config_.host);
        assert(!pg_config_.host.empty());

        pg_config_.user = get_env_var("TELEMETRY_DB_USER", pg_config_.user);
        assert(!pg_config_.user.empty());

        pg_config_.password = get_env_var("TELEMETRY_DB_PASSWORD", pg_config_.password);
        assert(!pg_config_.password.empty());
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

        const unsigned int WAIT_FOR_MQTT_SUBSCRIBE_S = 5;
        auto start = std::chrono::steady_clock::now();
        for(;;) {
            if (listener_->isSubscribed()) {
                break;
            }

            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
            assert(elapsed.count() < WAIT_FOR_MQTT_SUBSCRIBE_S);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        mqtt_client_ = std::make_shared<mqtt::async_client>(mqtt_config.server_uri, "benchmark_publisher");
        mqtt::connect_options conn_opts;
        conn_opts.set_keep_alive_interval(20);
        conn_opts.set_clean_session(true);
        conn_opts.set_automatic_reconnect(true);

        auto mqtt_pub_user = get_env_var("MQTT_PUB_USER", "");
        assert(!mqtt_pub_user.empty());
        conn_opts.set_user_name(mqtt_pub_user);

        auto mqtt_pub_password = get_env_var("MQTT_PUB_PASSWORD", "");
        assert(!mqtt_pub_password.empty());
        conn_opts.set_password(mqtt_pub_password);
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

        auto start = std::chrono::steady_clock::now();
        while (true) {
            long long current_count = get_db_row_count(*test_postgres_connection_);
            if (current_count >= expected_count) {
                break;
            }

            const unsigned int WAIT_FOR_MQTT_MESSAGES_IN_DB_TIMEOUT_S = 5;
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
            if (elapsed.count() >= WAIT_FOR_MQTT_MESSAGES_IN_DB_TIMEOUT_S) {
                std::stringstream error;
                error << "Timeout. Expected db messages count: " << expected_count <<
                        " and current count: " << current_count;
                state.SkipWithError(error.str());
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
