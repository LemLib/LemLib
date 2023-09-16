#pragma once

#include "logger.hpp"

#include "baseSink.hpp"

namespace lemlib {
template <typename... T> void Logger::log(Level level, fmt::format_string<T...> format, T&&... args) {
    for (BaseSink* sink : sinks) { sink->log(level, format, std::forward<T>(args)...); }
}

template <typename... T> void Logger::debug(fmt::format_string<T...> format, T&&... args) {
    log(Level::DEBUG, format, std::forward<T>(args)...);
}

template <typename... T> void Logger::info(fmt::format_string<T...> format, T&&... args) {
    log(Level::INFO, format, std::forward<T>(args)...);
}

template <typename... T> void Logger::warn(fmt::format_string<T...> format, T&&... args) {
    log(Level::WARN, format, std::forward<T>(args)...);
}

template <typename... T> void Logger::error(fmt::format_string<T...> format, T&&... args) {
    log(Level::ERROR, format, std::forward<T>(args)...);
}

template <typename... T> void Logger::fatal(fmt::format_string<T...> format, T&&... args) {
    log(Level::FATAL, format, std::forward<T>(args)...);
}
} // namespace lemlib