#pragma once

#include "baseSink.hpp"

#include "fmt/format.h"
#include "message.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include "pros/rtos.hpp"

namespace lemlib {
template <typename... T> void BaseSink::log(Level level, fmt::format_string<T...> format, T&&... args) {
    if (level < lowestLevel) { return; }

    // format the message first
    std::string message = fmt::format(format, std::forward<T>(args)...);

    uint32_t time = pros::millis();

    std::string formattedString = fmt::format(logFormat, fmt::arg("time", time), fmt::arg("level", level),
                                              fmt::arg("message", std::move(message)));

    logMessage(Message {std::move(formattedString), level, time});
}

template <typename... T> void BaseSink::debug(fmt::format_string<T...> format, T&&... args) {
    log(Level::DEBUG, format, std::forward<T>(args)...);
}

template <typename... T> void BaseSink::info(fmt::format_string<T...> format, T&&... args) {
    log(Level::INFO, format, std::forward<T>(args)...);
}

template <typename... T> void BaseSink::warn(fmt::format_string<T...> format, T&&... args) {
    log(Level::WARN, format, std::forward<T>(args)...);
}

template <typename... T> void BaseSink::error(fmt::format_string<T...> format, T&&... args) {
    log(Level::ERROR, format, std::forward<T>(args)...);
}

template <typename... T> void BaseSink::fatal(fmt::format_string<T...> format, T&&... args) {
    log(Level::FATAL, format, std::forward<T>(args)...);
}
} // namespace lemlib