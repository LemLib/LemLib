#pragma once

#include "abstractSink.hpp"

#include <memory>
#include <initializer_list>
#include <vector>

namespace lemlib {
/**
 * @brief Wrapper around std::vector<std::shared_ptr<AbstractSink>>
 *
 */
class Logger {
    public:
        /**
         * @brief Construct a new Logger object
         *
         * @param sinks The sinks to use.
         */
        Logger(std::initializer_list<std::shared_ptr<AbstractSink>> sinks);

        /**
         * @brief Log a message at the given level
         *
         * @tparam T The types of the varadic arguments
         * @param level The level to log at
         * @param format The format of the message
         * @param args The arguments for formatting
         */
        template <typename... T> void log(AbstractSink::Level level, fmt::format_string<T...> format, T&&... args);

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
    private:
        std::vector<std::shared_ptr<AbstractSink>> sinks;
};
} // namespace lemlib