#include "PostgresConfig.hpp"

namespace {
std::string escape_string(const std::string &input) {
    std::string result;
    result.reserve(input.size() * 2);
    for (char c : input) {
        if (c == '\\' || c == '\'') {
            result.push_back('\\');
        }
        result.push_back(c);
    }

    return result;
}

} // namespace

namespace Telemetry::Infrastructure {

std::string PostgresConfig::toConnectionString() const {
    return "host=" + host + " port=" + std::to_string(port) + " user=\'" +
            escape_string(user) + "\' password=\'" + escape_string(password) +
            "\' dbname=" + dbname;
}

} // namespace Telemetry::Infrastructure
