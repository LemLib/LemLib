#pragma once

#include "message.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include "pros/rtos.hpp"

namespace lemlib {
/**
 * @brief A base for any sink in LemLib to implement
 *
 */
class BaseSink {
    public:
        /**
         * @brief Set the lowest level
         *
         * @param level
         */
        void setLowestLevel(Level level);

        /**
         * @brief Set the format of the logger
         *
         * @param format
         */
        void setFormat(const std::string& format);

        /**
         * @brief Log a message at the given level
         *
         * @tparam T
         * @param level
         * @param format The format of the message
         * @param args
         */
        template <typename... T> void log(Level level, fmt::format_string<T...> format, T&&... args) {
            if (level < lowestLevel) { return; }

            // format the message first
            std::string message = fmt::format(format, std::forward<T>(args)...);

            uint32_t time = pros::millis();

            std::string formattedString = fmt::format(logFormat, fmt::arg("time", time), fmt::arg("level", level),
                                                      fmt::arg("message", std::move(message)));

            logMessage(Message {std::move(formattedString), level, time});
        }

        /**
         * @brief Log a message at the debug level
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> void debug(fmt::format_string<T...> format, T&&... args) {
            log(Level::DEBUG, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a message at the info level
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> void info(fmt::format_string<T...> format, T&&... args) {
            log(Level::INFO, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a message at the warn level
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> void warn(fmt::format_string<T...> format, T&&... args) {
            log(Level::WARN, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a message at the error level.
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> void error(fmt::format_string<T...> format, T&&... args) {
            log(Level::ERROR, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a message at the fatal level
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> void fatal(fmt::format_string<T...> format, T&&... args) {
            log(Level::FATAL, format, std::forward<T>(args)...);
        }
    protected:
        /**
         * @brief Log the given message
         *
         * @param message
         */
        virtual void logMessage(const Message& message) = 0;
    private:
        Level lowestLevel = Level::DEBUG;
        std::string logFormat;
};
} // namespace lemlib