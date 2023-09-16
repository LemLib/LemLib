#include "stdoutSink.hpp"

#include "fmt/core.h"
#include "fmt/color.h"

#include "message.hpp"

namespace lemlib {
StdoutSink* StdoutSink::sink = nullptr;

StdoutSink::StdoutSink() : task([&]() { loggingTask(); }) {}

void StdoutSink::setPrintRate(uint8_t printRate) { this->printRate = printRate; }

StdoutSink* StdoutSink::get() {
    if (sink == nullptr) { sink = new StdoutSink; }

    return sink;
}

void StdoutSink::logMessage(const Message& message) {
    mutex.take();
    buffer.push_back(message);
    mutex.give();
}

static fmt::color convertToColor(Level level) {
    switch (level) {
        case Level::INFO: return fmt::color::cyan;
        case Level::DEBUG: return fmt::color::green;
        case Level::WARN: return fmt::color::yellow;
        case Level::ERROR: return fmt::color::red;
        case Level::FATAL: return fmt::color::indian_red;
    }

    __builtin_unreachable();
}

void StdoutSink::loggingTask() {
    while (true) {
        mutex.take();
        if (buffer.size() > 0) {
            Message message = buffer.at(0);
            fmt::print(fmt::fg(convertToColor(message.level)), "{}\n", std::move(message.message));
            buffer.pop_front();
        }
        mutex.give();
        pros::delay(printRate);
    }
}
} // namespace lemlib