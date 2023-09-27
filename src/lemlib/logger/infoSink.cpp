#include "infoSink.hpp"

#include "bufferedSink.hpp"
#include "fmt/core.h"
#include "fmt/color.h"

#include "message.hpp"

namespace lemlib {
InfoSink::InfoSink() {}

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

void InfoSink::handleMessage(const Message& message) {
    fmt::print(fmt::fg(convertToColor(message.level)), "{}\033[0m", std::move(message.message));
}
} // namespace lemlib