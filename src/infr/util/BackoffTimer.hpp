#pragma once

#include <chrono>

class BackoffTimer {
public:
    BackoffTimer(
            std::chrono::seconds min_delay, std::chrono::seconds max_delay);
    void onSuccess();
    std::chrono::seconds getNextDelay();

private:
    std::chrono::seconds next_delay_;
    std::chrono::seconds min_delay_;
    std::chrono::seconds max_delay_;
};
