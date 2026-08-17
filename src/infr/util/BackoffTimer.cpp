#include "BackoffTimer.hpp"

#include <chrono>
#include <thread>

BackoffTimer::BackoffTimer(
        std::chrono::seconds min_delay, std::chrono::seconds max_delay)
    : min_delay_(min_delay), max_delay_(max_delay), next_delay_(min_delay) {}

void BackoffTimer::onSuccess() { next_delay_ = min_delay_; }

std::chrono::seconds BackoffTimer::getNextDelay() {
    auto result = next_delay_;
    next_delay_ *= 2;
    if (next_delay_ > max_delay_) {
        next_delay_ = max_delay_;
    }
    return result;
}
