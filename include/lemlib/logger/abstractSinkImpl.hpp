#pragma once

#include "lemlib/logger/abstractSink.hpp"

#include "lemlib/fmt/api.hpp"

#include "pros/rtos.hpp"

namespace lemlib {
inline const char* getColorFromLevel(lemlib::AbstractSink::Level level) {
    switch (level) {
        case lemlib::AbstractSink::Level::DEBUG: return "\033[0;36m";
        case lemlib::AbstractSink::Level::INFO: return "\033[0;32m";
        case lemlib::AbstractSink::Level::WARN: return "\033[0;33m";
        case lemlib::AbstractSink::Level::ERROR: return "\033[0;31m";
        case lemlib::AbstractSink::Level::FATAL: return "\033[0;31;2m";
    }
    __builtin_unreachable();
}

template <typename... T>
void AbstractSink::log(AbstractSink::Level level, fmt::format_string<T...> format, T&&... args) {
    if (level < lowestLevel) { return; }

    // format the message first
    std::string message = fmt::format(format, args...);

    // then we put that into the final message
    std::string formattedString =
        fmt::format(logFormat, fmt::arg("time", pros::millis()), fmt::arg("severity", level),
                    fmt::arg("message", message), fmt::arg("color", getColorFromLevel(level)));

    logString(AbstractSink::LoggableMessage {formattedString, level});
}

template <typename... T> void AbstractSink::debug(fmt::format_string<T...> format, T&&... args) {
    log(AbstractSink::Level::DEBUG, format, args...);
}

template <typename... T> void AbstractSink::info(fmt::format_string<T...> format, T&&... args) {
    log(AbstractSink::Level::INFO, format, args...);
}

template <typename... T> void AbstractSink::warn(fmt::format_string<T...> format, T&&... args) {
    log(AbstractSink::Level::WARN, format, args...);
}

template <typename... T> void AbstractSink::error(fmt::format_string<T...> format, T&&... args) {
    log(AbstractSink::Level::ERROR, format, args...);
}

template <typename... T> void AbstractSink::fatal(fmt::format_string<T...> format, T&&... args) {
    log(AbstractSink::Level::FATAL, format, args...);
}
} // namespace lemlib