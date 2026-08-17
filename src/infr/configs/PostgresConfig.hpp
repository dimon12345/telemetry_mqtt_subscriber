#pragma once

#include <string>

namespace Telemetry::Infrastructure {

struct PostgresConfig {
    std::string host = "pg-host";
    int port = 5432;
    std::string user = "postgres";
    std::string password = "password";
    std::string dbname = "telemetry";

    std::string toConnectionString() const;
};

} // namespace Telemetry::Infrastructure
