#pragma once

#include <atomic>

class CtrlCHandler {
private:
    CtrlCHandler();
    ~CtrlCHandler() = default;

public:
    CtrlCHandler(const CtrlCHandler&) = delete;
    CtrlCHandler& operator=(const CtrlCHandler&) = delete;
    static CtrlCHandler& getInstance();

    void wait();
    void handleSignal();
private:
    std::atomic<bool> signal_triggered_{false};
};
