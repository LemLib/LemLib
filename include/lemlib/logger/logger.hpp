#pragma once

#include "baseSink.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include <initializer_list>
#include <vector>

namespace lemlib {
/**
 * @brief Wrapper around std::vector<BaseSink*>
 *
 */
class Logger {
    public:
        /**
         * @brief Construct a new Logger object
         *
         * @param sink
         */
        Logger(std::initializer_list<BaseSink*> sinks);

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

        /**
         * @brief Get the default logger
         * 
         * @return Logger 
         */
        static Logger getDefaultLogger();
    private:
        std::vector<BaseSink*> sinks;
};
} // namespace lemlib

#define LEMLIB_DEBUG(format, ...) ::lemlib::Logger::getDefaultLogger().debug(format, __VA_ARGS__)
#define LEMLIB_INFO(format, ...) ::lemlib::Logger::getDefaultLogger().info(format, __VA_ARGS__)
#define LEMLIB_WARN(format, ...) ::lemlib::Logger::getDefaultLogger().warn(format, __VA_ARGS__)
#define LEMLIB_ERROR(format, ...) ::lemlib::Logger::getDefaultLogger().error(format, __VA_ARGS__)
#define LEMLIB_FATAL(format, ...) ::lemlib::Logger::getDefaultLogger().fatal(format, __VA_ARGS__)