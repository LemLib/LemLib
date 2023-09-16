#pragma once

#include "baseSink.hpp"

#include "message.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"
#include "fmt/args.h"

#include "pros/rtos.hpp"

namespace lemlib {
template <typename... T> void BaseSink::log(Level level, fmt::format_string<T...> format, T&&... args) {
    if (level < lowestLevel) { return; }

    // format the message first
    std::string message = fmt::format(format, std::forward<T>(args)...);

    // get the arguments
    fmt::dynamic_format_arg_store<fmt::format_context> formattingArgs = getExtraFormattingArgs();

    formattingArgs.push_back(fmt::arg("time", pros::millis()));
    formattingArgs.push_back(fmt::arg("level", level));
    formattingArgs.push_back(fmt::arg("message", message));

    std::string formattedString = fmt::vformat(logFormat, std::move(formattingArgs));

    logMessage(Message {std::move(formattedString), level});
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