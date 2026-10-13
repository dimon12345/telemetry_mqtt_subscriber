#pragma once

#include <string>


namespace Telemetry::Infrastructure {

struct PostgresConfig {
    std::string host = "pg-host";
    int port = 5432;
    std::string user = "postgres";
    std::string password = "postgres";
    std::string dbname = "telemetry";

    std::string toConnectionString() const {
        return "host=" + host +
               " port=" + std::to_string(port) +
               " user=" + user +
               " password=" + password +
               " dbname=" + dbname;
    }
};

} // namespace Telemetry::Infrastructure
