#include "ArgParseConfig.hpp"

#include <fstream>

#include <argparse.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

bool ArgParseConfig::loadFromArgs(int argc, char* argv[]) {
    argparse::ArgumentParser program("MQTT Subscriber");

    program.add_argument("-c", "--config")
            .help("Path to config.json file")
            .default_value(std::string("config.json"));

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Parse command line failed: " << e.what() << "\n";
        std::cerr << program;
        return false;
    }

    std::string config_path = program.get<std::string>("--config");
    if (!fs::exists(config_path)) {
        std::cout << "Config '" << config_path << "' not found.\n";
        if (saveDefaultConfig(config_path)) {
            return true;
        } else {
            std::cerr << "Default config '"+ config_path + "' save failed\n";
            return false;
        }
    }
    try {
        std::ifstream config_file(config_path);
        if (!config_file.is_open()) {
            std::cerr << "Can't open config file: " << config_path << "\n";
            return false;
        }

        json config_data;
        config_file >> config_data;

        auto pg_json = config_data.at("postgres");
        pg_config_.host = pg_json.at("host").get<std::string>();
        pg_config_.port = pg_json.at("port").get<int>();
        pg_config_.user = pg_json.at("user").get<std::string>();
        pg_config_.password = pg_json.at("password").get<std::string>();
        pg_config_.dbname = pg_json.at("dbname").get<std::string>();

        auto mqtt_json = config_data.at("mqtt");
        mqtt_config_.host = mqtt_json.at("host").get<std::string>();
        mqtt_config_.port = mqtt_json.at("port").get<int>();
        mqtt_config_.client_id = mqtt_json.at("client_id").get<std::string>();
        mqtt_config_.user = mqtt_json.at("user").get<std::string>();
        mqtt_config_.password = mqtt_json.at("password").get<std::string>();
        mqtt_config_.topic = mqtt_json.at("topic").get<std::string>();

        return true;
    } catch (const json::exception& e) {
        std::cerr << "Parse JSON Error (" << config_path << "): " << e.what() << "\n";
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Read config file Error: " << e.what() << "\n";
        return false;
    }
}

bool ArgParseConfig::saveDefaultConfig(const std::string& path) {
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
                {"host", mqtt_config_.host},
                {"port", mqtt_config_.port},
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
        std::cout << "Default config saved. Edit file config.json\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: Can't save config file: " << e.what() << "\n";
        return false;
    }
}
