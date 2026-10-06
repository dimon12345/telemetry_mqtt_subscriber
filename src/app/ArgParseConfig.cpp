#include <fstream>
#include <filesystem>

#include <argparse.hpp>
#include <nlohmann/json.hpp>

#include "ArgParseConfig.h"

namespace {
    const std::string AppName = "Event monitor";
    const std::string DefaultConfigFilename = "config.json";

    void parse_arguments(argparse::ArgumentParser &program, int argc, char **argv) {
        program.add_argument("-c", "--config")
                .default_value(DefaultConfigFilename)
                .help("config.json filename");
        program.add_argument("-p", "--disable-postgresql")
                .default_value(Config::DefaultDisablePostgreSql)
                .implicit_value(true)
                .help("disable PostgreSQL consumer");
        program.add_argument("-v", "--verbose")
                .default_value(Config::DefaultVerboseFlag)
                .implicit_value(true)
                .help("enables verbose");

        program.parse_args(argc, argv);
    }

}

ArgParseConfig::ArgParseConfig(int argc, char **argv) {
    argparse::ArgumentParser program(AppName);
    parse_arguments(program, argc, argv);

    verbose = program.get<bool>("--verbose");
    pg_disabled  = program.get<bool>("--disable-postgresql");

    std::string config_filename = program.get<std::string>("--config");
    if (!std::filesystem::exists(config_filename)) {
        return;
    }

    std::ifstream file(config_filename);

    // MQTT
    nlohmann::json config = nlohmann::json::parse(file);
    auto &mqtt_config = config["mqtt"];
    mqtt_host = mqtt_config["hostname"];
    mqtt_port = mqtt_config["port"];
    if (mqtt_port > std::numeric_limits<unsigned short>::max()) {
        throw std::runtime_error("Wrong port number: " + std::to_string(mqtt_port));
    }
    mqtt_topic = mqtt_config["topic"];

    // PostgreSQL
    if (!pg_disabled) {
        pg_host = config["pg"]["hostname"];
    }
}
