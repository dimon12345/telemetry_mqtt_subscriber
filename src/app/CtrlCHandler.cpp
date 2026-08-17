#include <csignal>

#include "CtrlCHandler.h"

static void signal_handler(int signal) {
    if (signal != SIGINT) {
        return;
    }

    CtrlCHandler::getInstance().handleSignal();
}

CtrlCHandler::CtrlCHandler() {
    std::signal(SIGINT, signal_handler);
}

CtrlCHandler& CtrlCHandler::getInstance() {
    static CtrlCHandler instance;
    return instance;
}

void CtrlCHandler::wait() {
    signal_triggered_.wait(false);
}

void CtrlCHandler::handleSignal() {
    signal_triggered_ = true;
    signal_triggered_.notify_all();
}
