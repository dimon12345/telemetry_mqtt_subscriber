#include "SignalManager.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

namespace {
    std::atomic<bool> signal_received_flag{false};

    static void signalHandler(int signal) {
        if (signal != SIGINT && signal != SIGTERM) {
            return;
        }

        signal_received_flag.store(true);
    }
}

SignalManager::SignalManager() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
}

SignalManager& SignalManager::instance() {
    static SignalManager manager_instance;
    return manager_instance;
}

void SignalManager::waitForSignal() {
    while(!signal_received_flag.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
