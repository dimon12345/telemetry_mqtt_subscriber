#pragma once

#include "Config.h"

class ArgParseConfig : public Config {
public:
    ArgParseConfig(int argc, char **argv);
};
