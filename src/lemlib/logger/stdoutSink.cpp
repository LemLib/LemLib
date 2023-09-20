#include "stdoutSink.hpp"

#include "bufferedSink.hpp"
#include "fmt/core.h"
#include "fmt/color.h"

#include "message.hpp"

namespace lemlib {
StdoutSink* StdoutSink::sink = nullptr;

StdoutSink::StdoutSink() : BufferedSink() {}

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

void StdoutSink::handleMessage(const Message& message) {
    if (colorMode) {
        fmt::print(fmt::fg(convertToColor(message.level)), "{}\n\033[0m", std::move(message.message));
    } else {
        fmt::print("{}\n", std::move(message.message));
    }
}
} // namespace lemlib