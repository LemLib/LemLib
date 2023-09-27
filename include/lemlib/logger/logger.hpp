#pragma once

#include "baseSink.hpp"

#include "infoSink.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include <memory>
#include <array>

namespace lemlib {
class Logger {
    public:
        /**
         * @brief Get the info sink object
         *
         * @return InfoSink
         */
        std::shared_ptr<InfoSink> getInfoSink();

        /**
         * @brief Log to the info logger
         *
         * @tparam T
         * @param level
         * @param format
         * @param args
         */
        template <typename... T> void log(Level level, fmt::format_string<T...> format, T&&... args);

        /**
         * @brief Log a message at the debug level to the info logger
         *
         * @tparam T
         * @param level
         * @param format
         * @param args
         */
        template <typename... T> void logDebug(fmt::format_string<T...> format, T&&... args);

        /**
         * @brief Log a message at the info level to the info logger
         *
         * @tparam T
         * @param level
         * @param format
         * @param args
         */
        template <typename... T> void logInfo(fmt::format_string<T...> format, T&&... args);

        /**
         * @brief Log a message at the error level to the info logger
         *
         * @tparam T
         * @param level
         * @param format
         * @param args
         */
        template <typename... T> void logWarn(fmt::format_string<T...> format, T&&... args);

        /**
         * @brief Log a message at the warn level to the info logger
         *
         * @tparam T
         * @param level
         * @param format
         * @param args
         */
        template <typename... T> void logError(fmt::format_string<T...> format, T&&... args);

        /**
         * @brief Log a message at the fatal level to the info logger
         *
         * @tparam T
         * @param level
         * @param format
         * @param args
         */
        template <typename... T> void logFatal(fmt::format_string<T...> format, T&&... args);
    private:
        static std::shared_ptr<InfoSink> infoSink;
};

template <typename... T> void Logger::log(Level level, fmt::format_string<T...> format, T&&... args) {
    getInfoSink()->log(level, format, std::forward<T>(args)...);
}

template <typename... T> void Logger::logDebug(fmt::format_string<T...> format, T&&... args) {
    log(Level::DEBUG, format, std::forward<T>(args)...);
}

template <typename... T> void Logger::logInfo(fmt::format_string<T...> format, T&&... args) {
    log(Level::INFO, format, std::forward<T>(args)...);
}

template <typename... T> void Logger::logWarn(fmt::format_string<T...> format, T&&... args) {
    log(Level::WARN, format, std::forward<T>(args)...);
}

template <typename... T> void Logger::logError(fmt::format_string<T...> format, T&&... args) {
    log(Level::ERROR, format, std::forward<T>(args)...);
}

template <typename... T> void Logger::logFatal(fmt::format_string<T...> format, T&&... args) {
    log(Level::FATAL, format, std::forward<T>(args)...);
}
} // namespace lemlib