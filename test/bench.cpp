#include <benchmark/benchmark.h>

#include <string>
#include <chrono>
#include <thread>
#include <memory>

#include <mqtt/async_client.h>
#include <pqxx/pqxx>

#include "Config.hpp"
#include "core/Sensors.hpp"
#include "infr/database/PqxxSensorRepository.hpp"
#include "infr/services/MqttTelemetryService.hpp"
#include "infr/services/SensorValueService.hpp"

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
        pqxx::result res = tx.exec("SELECT COUNT(*) FROM measurements;");
        return res[0][0].as<long long>();
    }

    void clear_db(pqxx::connection& conn) {
        pqxx::work tx(conn);
        tx.exec("TRUNCATE TABLE measurements;");
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
        const auto &postgres_config = config_.getPostgres();
        auto db_connection_string = postgres_config.toConnectionString();
        std::cout << "connection string: " << db_connection_string << std::endl;

        auto connection = std::make_shared<pqxx::connection>(db_connection_string);
        auto sensor_repository = std::make_shared<PqxxSensorRepository>(std::move(connection));
        auto sensor_value_service = std::make_shared<SensorValueService>(std::move(sensor_repository));
        mqtt_telemetry_service_ = std::make_shared<MqttTelemetryService>(std::move(sensor_value_service));

        mqtt_telemetry_service_->start();

        const auto &mqtt_config = config_.getMqttConfig();
        mqtt_client_ = std::make_shared<mqtt::async_client>(mqtt_config.server_uri, mqtt_config.client_id);
        mqtt_client_->set_callback(*mqtt_telemetry_service_);

        mqtt::connect_options conn_opts;
        conn_opts.set_keep_alive_interval(20);
        conn_opts.set_clean_session(true);
        conn_opts.set_automatic_reconnect(true);
        conn_opts.set_user_name(mqtt_config.user);
        conn_opts.set_password(mqtt_config.password);

        mqtt_client_->connect(conn_opts)->wait();

        constexpr int QOS = 1;
        mqtt_client_->subscribe(mqtt_config.topic, QOS)->wait();

        test_postgres_connection_ = std::make_shared<pqxx::connection>(db_connection_string);
        clear_db(*test_postgres_connection_);
    }

    void TearDown(const ::benchmark::State& state) override {
        mqtt_telemetry_service_->stop();
    }

    ~MqttSingleConnFixture() {
        if (mqtt_client_ && mqtt_client_->is_connected()) {
            try {
                mqtt_client_->disconnect()->wait();
                std::cout << "disconnected from mqtt" << std::endl;
                mqtt_client_.reset();
            } catch (...) {
            }
        }
    }

private:
    std::shared_ptr<MqttTelemetryService> mqtt_telemetry_service_;
};

BENCHMARK_DEFINE_F(MqttSingleConnFixture, SendViaSingleConnection)(benchmark::State& state) {
    std::cout << "start test" << std::endl;
    const int num_messages = state.range(0);
    const auto &topic = config_.getMqttConfig().topic;

    for (auto _ : state) {
        long long start_count = get_db_row_count(*test_postgres_connection_);
        long long expected_count = start_count + num_messages;

        for (int i = 0; i < num_messages; ++i) {
            std::string payload = "0";
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
    ->Iterations(5);

BENCHMARK_MAIN();
