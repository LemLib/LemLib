#pragma once

#include "abstractSink.hpp"

#include "lemlib/logger/logger.hpp"

namespace lemlib {
template <typename... T> void Logger::log(AbstractSink::Level level, fmt::format_string<T...> format, T&&... args) {
    for (std::shared_ptr<AbstractSink> sink : sinks) { sink->log(level, format, args...); }
}

template <typename... T> void Logger::debug(fmt::format_string<T...> format, T&&... args) {
    log(AbstractSink::Level::DEBUG, format, args...);
}

template <typename... T> void Logger::info(fmt::format_string<T...> format, T&&... args) {
    log(AbstractSink::Level::INFO, format, args...);
}

template <typename... T> void Logger::warn(fmt::format_string<T...> format, T&&... args) {
    log(AbstractSink::Level::WARN, format, args...);
}

template <typename... T> void Logger::error(fmt::format_string<T...> format, T&&... args) {
    log(AbstractSink::Level::ERROR, format, args...);
}

template <typename... T> void Logger::fatal(fmt::format_string<T...> format, T&&... args) {
    log(AbstractSink::Level::FATAL, format, args...);
}
} // namespace lemlib