#pragma once

#include <string>

#include "lemlib/fmt/api.hpp"

namespace lemlib {
/**
 * @brief Base for any logger in LemLib to implement.
 *
 */
class AbstractLogger {
    public:
        /*
         * @brief A level enumeration.
         *
         * Debug: Only enabled if the lowest logging level is DEBUG
         * Info: General information
         * Warn: Warnings, usually not critical/doesn't affect the robot
         * Error: Errors, usually critical and affects the robot
         * Fatal: Fatal errors, crashes the program
         *
         * @note The log level is inclusive. For example, if the log level is set to INFO, all logs of INFO and
         * higher will be logged.
         */
        enum class Level { DEBUG, INFO, WARN, ERROR, FATAL };

        /**
         * @brief A loggable message.
         *
         */
        struct LoggableMessage {
                std::string message;
                Level level; // we store the level for other purposes
        };

        /**
         * @brief Set the lowest level the logger can log at
         *
         * @param lowestLevel The lowest level
         */
        void setLowestLevel(Level lowestLevel);

        /**
         * @brief Set the format of the logger.
         *
         */
        void setFormat(const std::string& logFormat);

        /**
         * @brief Log a message at the given level
         *
         * @tparam T The types of the varadic arguments
         * @param level The level to log at
         * @param format The format of the message
         * @param args The arguments for formatting
         */
        template <typename... T> void log(Level level, fmt::format_string<T...> format, T&&... args);

        /**
         * @brief Log a message at the debug level
         *
         * @tparam T The types of the varadic arguments
         * @param format The level to log at
         * @param args The arguments for formatting
         */
        template <typename... T> void debug(fmt::format_string<T...> format, T&&... args);

        /**
         * @brief Log a message at the info level
         *
         * @tparam T The types of the varadic arguments
         * @param format The level to log at
         * @param args The arguments for formatting
         */
        template <typename... T> void info(fmt::format_string<T...> format, T&&... args);

        /**
         * @brief Log a message at the warn level
         *
         * @tparam T The types of the varadic arguments
         * @param format The level to log at
         * @param args The arguments for formatting
         */
        template <typename... T> void warn(fmt::format_string<T...> format, T&&... args);

        /**
         * @brief Log a message at the error level.
         *
         * @tparam T The types of the varadic arguments
         * @param format The level to log at
         * @param args The arguments for formatting
         */
        template <typename... T> void error(fmt::format_string<T...> format, T&&... args);

        /**
         * @brief Log a message at the fatal level
         *
         * @tparam T The types of the varadic arguments
         * @param format The level to log at
         * @param args The arguments for formatting
         */
        template <typename... T> void fatal(fmt::format_string<T...> format, T&&... args);
    protected:
        /**
         * @brief Log the given string
         *
         * @param message The message to log
         */
        virtual void logString(const LoggableMessage& message) = 0;
    private:
        Level lowestLevel = Level::DEBUG;
        std::string logFormat;
};
} // namespace lemlib