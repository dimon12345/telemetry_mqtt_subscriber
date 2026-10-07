#pragma once

class SignalManager {
private:
    SignalManager();
    ~SignalManager() = default;

public:
    SignalManager(const SignalManager&) = delete;
    SignalManager& operator=(const SignalManager&) = delete;
    static SignalManager& instance();

    void waitForSignal();
};
