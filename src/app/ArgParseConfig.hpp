#pragma once

#include "Config.hpp"

class ArgParseConfig : public Config {
public:
    ArgParseConfig(int argc, char* argv[]);

private:
    bool saveConfig(const std::string& path);
    void loadSecuredValues();
};
