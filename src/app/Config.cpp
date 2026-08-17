#include <fstream>
#include <filesystem>

#include <argparse.hpp>
#include <nlohmann/json.hpp>

#include "Config.h"

static void parse_arguments(argparse::ArgumentParser &program, int argc, char **argv) {
    program.add_argument("-c", "--config")
            .default_value(Config::DefaultConfigFilename)
            .help("config.json filename");
    program.add_argument("-v", "--verbose")
            .default_value(Config::DefaultVerboseFlag)
            .implicit_value(true)
            .help("print mqtt messages");

    program.parse_args(argc, argv);
}

Config::Config(int argc, char **argv) {
    argparse::ArgumentParser program(Config::AppName);
    parse_arguments(program, argc, argv);

    verbose_ = program.get<bool>("--verbose");

    std::string config_filename = program.get<std::string>("--config");
    if (!std::filesystem::exists(config_filename)) {
        return;
    }

    std::ifstream file(config_filename);
    nlohmann::json config = nlohmann::json::parse(file);
    mqtt_host_ = config["mqtt"]["hostname"];
    mqtt_port_ = config["mqtt"]["port"];
    pg_host_ = config["pg"]["hostname"];

    validate();
}

void Config::validate() {
    if (mqtt_port_ > std::numeric_limits<unsigned short>::max()) {
        throw std::runtime_error("Wrong port number: " + std::to_string(mqtt_port_));
    }
}
