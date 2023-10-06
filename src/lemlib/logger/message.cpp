#include "message.hpp"

namespace lemlib {
Message::Message(Level i_level, std::string i_message) {
    level = i_level;
    message = i_message;
};

const std::string RESET_ANSI = "\033[0m";

std::string getColor(Level level) {
    switch (level) {
        case Level::Info: return "\033[0;36m"; // cyan
        case Level::Debug: return "\033[0;32m"; // green
        case Level::Warn: return "\033[0;33m"; // yellow
        case Level::Error: return "\033[0;31m"; // red
        case Level::Fatal: return "\033[0;31;2m"; // dark red
        case Level::Telemetry: return "";
    }
    __builtin_unreachable();
}

std::string Message::getLevelString(Level level, bool isColored) {
    std::string levelStr;
    switch (level) {
        case Level::Info: levelStr = "INFO"; break;
        case Level::Debug: levelStr = "DEBUG"; break;
        case Level::Warn: levelStr = "WARN"; break;
        case Level::Error: levelStr = "ERROR"; break;
        case Level::Fatal: levelStr = "FATAL"; break;
        case Level::Telemetry: levelStr = "TELEMETRY"; break;
    }

    if (isColored) {
        return getColor(level) + levelStr + RESET_ANSI;
    } else {
        return levelStr;
    }
}

} // namespace lemlib
