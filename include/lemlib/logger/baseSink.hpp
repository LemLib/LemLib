#pragma once

#include "message.hpp"

#include "fmt/core.h"
#include "fmt/args.h"

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
         * @brief Log the given message
         *
         * @param message
         */
        virtual void logMessage(const Message& message);

        /**
         * @brief Get the arguments for formatting
         * 
         * @return fmt::dynamic_format_arg_store<fmt::format_context> 
         */
        virtual fmt::dynamic_format_arg_store<fmt::format_context> getExtraFormattingArgs();
    private:
        Level lowestLevel = Level::DEBUG;
        std::string logFormat = "[LemLib] {level}: {message}";
};
} // namespace lemlib