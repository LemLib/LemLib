#pragma once

#include <initializer_list>
#include "pros/rtos.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"
#include "fmt/args.h"

#include "lemlib/logger/message.hpp"

namespace lemlib {
/**
 * @brief A base for any sink in LemLib to implement.
 *
 * Sinks are LemLib's abstraction for destinations that logged messages can be sent to. They are the backbone of the
 * logging implementation. A sink could send information to anything, to stdout, to a file, or even the UI on the
 * brain screen.
 */
class BaseSink {
    public:
        BaseSink() = default;

        /**
         * @brief Construct a new combined sink
         *
         * <h3> Example Usage </h3>
         * @code
         * lemlib::BaseSink combinedSink({lemlib::telemetrySink(), lemlib::infoSink()});
         * combinedSink.info("This will be sent to both sinks!");
         * @endcode
         *
         * @param sinks The sinks that will have messages sent to them when
         */
        BaseSink(std::initializer_list<std::shared_ptr<BaseSink>> sinks);

        /**
         * @brief Set the lowest level.
         * If this is a combined sink, this operation will
         * apply for all the parent sinks.
         * @param level
         *
         * If messages are logged that are below the lowest level, they will be ignored.
         * The hierarchy of the levels is as follows:
         * - INFO
         * - DEBUG
         * - WARN
         * - ERROR
         * - FATAL
         */
        void setLowestLevel(Level level);

        /**
         * @brief Log a message at the given level
         * If this is a combined sink, this operation will
         * apply for all the parent sinks.
         *
         * @tparam T
         * @param level The level at which to send the message.
         * @param format The format that the message will use. Use "{}" as placeholders.
         * @param args The values that will be substituted into the placeholders in the format.
         *
         * <h3> Example Usage </h3>
         * @code
         * sink.log(lemlib::Level::INFO, "{} from the logger!", "Hello");
         * @endcode

         */
        template <typename... T> void log(Level level, fmt::format_string<T...> format, T&&... args) {
            if (!this->sinks.empty()) {
                for (std::shared_ptr<BaseSink> sink : this->sinks) {
                    sink->log(level, format, std::forward<T>(args)...);
                }
                return;
            }

            if (level < this->lowestLevel) { return; }

            // substitute the user's arguments into the format.
            std::string messageString = fmt::format(format, std::forward<T>(args)...);

            Message message = Message {.level = level, .time = pros::millis()};

            // get the arguments
            fmt::dynamic_format_arg_store<fmt::format_context> formattingArgs = getExtraFormattingArgs(message);

            formattingArgs.push_back(fmt::arg("time", message.time));
            formattingArgs.push_back(fmt::arg("level", message.level));
            formattingArgs.push_back(fmt::arg("message", messageString));

            std::string formattedString = fmt::vformat(this->logFormat, std::move(formattingArgs));
            message.message = std::move(formattedString);
            sendMessage(std::move(message));
        }

        /**
         * @brief Log a message at the debug level.
         * If this is a combined sink, this operation will
         * apply for all the parent sinks.
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> void debug(fmt::format_string<T...> format, T&&... args) {
            log(Level::DEBUG, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a message at the info level
         * If this is a combined sink, this operation will
         * apply for all the parent sinks.
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> void info(fmt::format_string<T...> format, T&&... args) {
            log(Level::INFO, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a message at the warn level
         * If this is a combined sink, this operation will
         * apply for all the parent sinks.
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> void warn(fmt::format_string<T...> format, T&&... args) {
            log(Level::WARN, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a message at the error level.
         * If this is a combined sink, this operation will
         * apply for all the parent sinks.
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> void error(fmt::format_string<T...> format, T&&... args) {
            log(Level::ERROR, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a message at the fatal level
         * If this is a combined sink, this operation will
         * apply for all the parent sinks.
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
        virtual void sendMessage(const Message& message);

        /**
         * @brief Set the format of messages that the sink sends
         *
         * @param format
         *
         * Changing the format of the sink changes the way each logged message looks. The following named formatting
         * specifiers can be used:
         * - {time} The time the message was sent in milliseconds since the program started.
         * - {level} The level of the logged message.
         * - {message} The message itself.
         *
         * <h3> Example Usage </h3>
         * @code
         * infoSink()->setFormat("[LemLib] -- {time} -- {level}: {Message}");
         * infoSink()->info("hello");
         * // This will be formatted as:
         * // "[LemLIb] -- 10 -- INFO:  hello"
         * @endcode
         */
        void setFormat(const std::string& format);

        /**
         * @brief Get the extra named arguments for formatting
         *
         * Can be overridden to add extra named arguments to the sink's format.
         * <h3> Example Usage </h3>
         *
         * The following code would add a {zero} formatting argument which could be used in setFormat method of this
         * sink.
         *
         * @code
         * fmt::dynamic_format_arg_store<fmt::format_context>
         * ExampleSink::getExtraFormattingArgs(const Message& messageInfo) {
         *     fmt::dynamic_format_arg_store<fmt::format_context> args;
         *      args.push_back(fmt::arg("zero", 0);
         *      return args;
         * }
         * @endcode
         *
         * @return fmt::dynamic_format_arg_store<fmt::format_context>
         */
        virtual fmt::dynamic_format_arg_store<fmt::format_context> getExtraFormattingArgs(const Message& messageInfo);
    private:
        Level lowestLevel = Level::DEBUG;
        std::string logFormat;

        std::vector<std::shared_ptr<BaseSink>> sinks {};
};
} // namespace lemlib
