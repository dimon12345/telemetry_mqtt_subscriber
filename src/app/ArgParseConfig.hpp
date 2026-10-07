#pragma once

#include "Config.hpp"

class ArgParseConfig : public Config {
public:
    bool loadFromArgs(int argc, char* argv[]);

private:
    bool saveDefaultConfig(const std::string& path);
    void loadSecuredValues();
};
