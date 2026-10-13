#include <iostream>
#include <fstream>
#include <string>

#include <argparse.hpp>
#include <nlohmann/json.hpp>

#include "ArgParseConfig.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace {
    std::string get_env_var(const std::string& key) {
        if (const char* val = std::getenv(key.c_str())) {
            return val;
        }
        return "";
    }
}

ArgParseConfig::ArgParseConfig(int argc, char* argv[]) {
    argparse::ArgumentParser program("MQTT Subscriber");

    program.add_argument("-c", "--config")
            .help("Path to config.json file")
            .default_value(std::string("config.json"));

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Parse command line failed: ") + e.what());
    }

    std::string config_path = program.get<std::string>("--config");
    if (!fs::exists(config_path)) {
        std::cerr << "Config '" << config_path << "' not found.\n";
        if (saveConfig(config_path)) {
            loadSecuredValues();
            return;
        } else {
            throw std::runtime_error(std::string("Default config '") + config_path + "' save failed");
        }
    }
    try {
        std::ifstream config_file(config_path);
        if (!config_file.is_open()) {
            throw("Can't open config file: " + config_path);
        }

        json config_data;
        config_file >> config_data;

        auto mqtt_json = config_data.at("mqtt");
        mqtt_config_.server_uri = mqtt_json.at("server_uri").get<std::string>();
        mqtt_config_.client_id = mqtt_json.at("client_id").get<std::string>();
        mqtt_config_.user = mqtt_json.at("user").get<std::string>();
        mqtt_config_.password = mqtt_json.at("password").get<std::string>();
        mqtt_config_.topic = mqtt_json.at("topic").get<std::string>();

        auto pg_json = config_data.at("postgres");
        pg_config_.host = pg_json.at("host").get<std::string>();
        pg_config_.port = pg_json.at("port").get<int>();
        pg_config_.user = pg_json.at("user").get<std::string>();
        pg_config_.password = pg_json.at("password").get<std::string>();
        pg_config_.dbname = pg_json.at("dbname").get<std::string>();

        loadSecuredValues();
    } catch (const json::exception& e) {
        throw std::runtime_error("Parse JSON Error '" + config_path + "': " + e.what());
    }
}

bool ArgParseConfig::saveConfig(const std::string& path) {
    try {
        json config_json;
        config_json["postgres"] = {
                {"host", pg_config_.host},
                {"port", pg_config_.port},
                {"user", pg_config_.user},
                {"password", pg_config_.password},
                {"dbname", pg_config_.dbname}
        };
        config_json["mqtt"] = {
                {"server_uri", mqtt_config_.server_uri},
                {"client_id", mqtt_config_.client_id},
                {"user", mqtt_config_.user},
                {"password", mqtt_config_.password},
                {"topic", mqtt_config_.topic}
        };

        std::ofstream config_file(path);
        if (!config_file.is_open()) {
            std::cerr << "Error: Can't create config file: " << path << "\n";
            return false;
        }

        config_file << config_json.dump(4);
        std::cerr << "Default config saved. Please, edit file config.json\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: Can't save config file: " << e.what() << "\n";
        return false;
    }
}

void ArgParseConfig::loadSecuredValues() {
    auto mqtt_user = get_env_var("MQTT_USER");
    if (!mqtt_user.empty()) {
        mqtt_config_.user = mqtt_user;
    }
    auto mqtt_password = get_env_var("MQTT_PASSWORD");
    if (!mqtt_password.empty()) {
        mqtt_config_.password = mqtt_password;
    }

    auto pg_user = get_env_var("POSTGRES_USER");
    if (!pg_user.empty()) {
        pg_config_.user = pg_user;
    }
    auto pg_password = get_env_var("POSTGRES_PASSWORD");
    if (!pg_password.empty()) {
        pg_config_.password = pg_password;
    }
}
