#pragma once

#include <iomanip>

namespace Telemetry::Common {

std::string to_iso8601(std::chrono::system_clock::time_point tp) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    struct tm bt;
#if defined(_WIN32) || defined(_WIN64)
    gmtime_s(&bt, &time);
#else
    gmtime_r(&time, &bt);
#endif
    std::ostringstream oss;
    oss << std::put_time(&bt, "%FT%TZ");
    return oss.str();
}

} // namespace Telemetry::Common
