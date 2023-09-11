#pragma once

#include "lemlib/logger/abstractLogger.hpp"

#include "lemlib/fmt/api.hpp"

#include "pros/rtos.hpp"

namespace lemlib {
template <typename... T>
void AbstractLogger::log(AbstractLogger::Level level, fmt::format_string<T...> format, T&&... args) {
    if (level < lowestLevel) { return; }

    // format the message first
    std::string message = fmt::format(format, args...);

    // then we put that into the final message
    std::string formattedString = fmt::format(logFormat, fmt::arg("time", pros::millis()), fmt::arg("severity", level),
                                              fmt::arg("message", message));

    logString(AbstractLogger::LoggableMessage {formattedString, level});
}

template <typename... T> void AbstractLogger::debug(fmt::format_string<T...> format, T&&... args) {
    log(AbstractLogger::Level::DEBUG, format, args...);
}

template <typename... T> void AbstractLogger::info(fmt::format_string<T...> format, T&&... args) {
    log(AbstractLogger::Level::INFO, format, args...);
}

template <typename... T> void AbstractLogger::warn(fmt::format_string<T...> format, T&&... args) {
    log(AbstractLogger::Level::WARN, format, args...);
}

template <typename... T> void AbstractLogger::error(fmt::format_string<T...> format, T&&... args) {
    log(AbstractLogger::Level::ERROR, format, args...);
}

template <typename... T> void AbstractLogger::fatal(fmt::format_string<T...> format, T&&... args) {
    log(AbstractLogger::Level::FATAL, format, args...);
}
} // namespace lemlib