#pragma once

#include "Config.hpp"

class ArgParseConfig : public Config {
public:
    ArgParseConfig(int argc, const char *const *argv);

private:
    bool saveConfig(const std::string &path);
    void loadEnvironmentValues();
};
