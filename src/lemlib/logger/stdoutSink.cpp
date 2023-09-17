#include "stdoutSink.hpp"

#include "fmt/core.h"
#include "fmt/color.h"

#include "message.hpp"

namespace lemlib {
StdoutSink* StdoutSink::sink = nullptr;

StdoutSink::StdoutSink() : task([&]() { loggingTask(); }) {}

void StdoutSink::setPrintRate(uint8_t printRate) { this->printRate = printRate; }

/**
 * @brief Set the color mode (true enables, false disables)
 *
 * @param mode
 */
void StdoutSink::flipColorMode(bool mode) { this->colorMode = mode; }

StdoutSink* StdoutSink::get() {
    if (sink == nullptr) { sink = new StdoutSink; }

    return sink;
}

void StdoutSink::logMessage(const Message& message) {
    mutex.take();
    buffer.push_back(message);
    mutex.give();
}

static fmt::terminal_color convertToColor(Level level) {
    switch (level) {
        case Level::INFO: return fmt::terminal_color::cyan;
        case Level::DEBUG: return fmt::terminal_color::green;
        case Level::WARN: return fmt::terminal_color::yellow;
        case Level::ERROR: return fmt::terminal_color::red;
        case Level::FATAL: return fmt::terminal_color::bright_red;
    }

    __builtin_unreachable();
}

void StdoutSink::loggingTask() {
    while (true) {
        mutex.take();
        if (buffer.size() > 0) {
            Message message = buffer.at(0);
            if (colorMode) {
                fmt::print(fmt::fg(convertToColor(message.level)), "{}\n\033[0m", std::move(message.message));
            } else {
                fmt::print("{}\n", std::move(message.message));
            }
            buffer.pop_front();
        }
        mutex.give();
        pros::delay(printRate);
    }
}
} // namespace lemlib