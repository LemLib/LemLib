#pragma once

#include "message.hpp"
#include "buffer.hpp"
#include <iostream>

#define FMT_HEADER_ONLY
#include "fmt/core.h"
#include "fmt/format.h"

namespace lemlib {
/**
 * @brief LemLib's logging implementation. This class is static meaning that you can use it's methods without having to
 * instantiate the class.
 */
class Logger {
    public:
        /**
         * @brief Sets the lowest level of logging that can be logged. Levels lower than this will not be printed. The
         * hierarchy is as follows:
         * - Info
         * - Debug
         * - Warn
         * - Error
         * - Fatal
         * - Telemetry
         * @note The default lowest level is Info.
         * @param level The new lowest level.
         */
        static void setLowestLevel(Level level);

        /**
         * @brief Sets the format for logged messages. This will not affect the telemetry messages.
         * @param format The new format. You can use the following named formatting specifiers:
         * - {level} The level of the log.
         * - {time} The time in milliseconds since the program started.
         * - {message} This is the actual content of the log
         * @note The default format is "[Lemlib] {level}: {message}"
         */
        static void setFormat(const std::string& format);

        /**
         * @brief Sets the format for telemetry messages. This will not affect the other messages.
         * @param format The new format. You can use the following named formatting specifiers:
         * - {level} The level of the log.
         * - {time} The time in milliseconds since the program started.
         * - {message} This is the actual content of the log
         * @note The default format is "TELEMSTART{message}TELEMEND"
         */
        static void setTelemetryFormat(const std::string& format);

        /**
         * @brief Sets whether the logging output should be colored. This only affects the {level} part of the message.
         * This will not affect telemetry messages.
         * @param colored A value of true will color the messages, a value of false will not.
         */
        static void setColored(bool colored);

        /**
         * @brief Log a message at the given level. Generally you would not want to use this directly, use one of the
         * methods with the level already applied.
         *
         * @tparam T
         * @param level The level of the logged message.
         * @param format The format of the message. Use {} as a placeholder for the values specified in args.
         * @param args The arguments that will be placed into the format.
         *
         * @example
         *
         * ```cpp
         * lemlib::Logger::log(lemlib::Level::Info, "Hello {}.", "world");
         * // This will print "[LemLib] INFO: Hello world." to the terminal.
         * ```
         */
        template <typename... T> static void log(Level level, fmt::format_string<T...> format, T&&... args) {
            if (level < lowestLevel) { return; }

            std::string message = fmt::format(format, std::forward<T>(args)...); // Apply the user's formatting here

            std::string levelString = Message::getLevelString(level, true);
            uint32_t time = pros::millis();

            std::string message_format;

            if (level == Level::Telemetry) {
                message_format = telemetryFormat;
            } else {
                message_format = logFormat;
            }

            std::string formatted_message =
                fmt::format(message_format, fmt::arg("time", time), fmt::arg("level", levelString),
                            fmt::arg("message", std::move(message))); // Apply the global formatting

            Message payload(level, formatted_message);
            Buffer::getBuffer().push(payload);
        };

        /**
         * @brief Log a message at the info level
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> static void info(fmt::format_string<T...> format, T&&... args) {
            Logger::log(Level::Info, format, args...);
        };

        template <typename... T> static void debug(fmt::format_string<T...> format, T&&... args) {
            Logger::log(Level::Debug, format, args...);
        };

        template <typename... T> static void warn(fmt::format_string<T...> format, T&&... args) {
            Logger::log(Level::Warn, format, args...);
        };

        template <typename... T> static void error(fmt::format_string<T...> format, T&&... args) {
            Logger::log(Level::Error, format, args...);
        };

        template <typename... T> static void fatal(fmt::format_string<T...> format, T&&... args) {
            Logger::log(Level::Fatal, format, args...);
        };

        template <typename... T> static void telemetry(fmt::format_string<T...> format, T&&... args) {
            Logger::log(Level::Telemetry, format, args...);
        };
    private:
        static Level lowestLevel;
        static std::string logFormat;
        static std::string telemetryFormat;
        static bool isColored;
};

} // namespace lemlib
