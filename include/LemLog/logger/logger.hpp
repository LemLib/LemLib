#pragma once

#include "fmtlib/core.h"
#include <string>

namespace logger {

/**
 * @brief add a topic to the whitelist
 *
 * Debug messages need their topic on the whitelist in order to be sent to the
 * sinks
 *
 * @param s the topic to add
 *
 * @b Example:
 * @code {.cpp}
 * void initialize() {
 *   logger::addWhitelist("lemlib/motions/boomerang");
 *   logger::addWhitelist("lemlib/motions/follow");
 * }
 * @endcode
 */
void addWhitelist(std::string s);

/**
 * @brief remove a topic from the whitelist
 *
 * Debug messages need their topic on the whitelist in order to be sent to the
 * sinks
 *
 * @param s the topic to remove
 *
 * @b Example:
 * @code {.cpp}
 * void initialize() {
 *   logger::removeWhitelist("lemlib/motions/boomerang");
 *   logger::removeWhitelist("lemlib/motions/follow");
 * }
 * @endcode
 */
void removeWhitelist(std::string s);

/**
 * @brief add a topic to the blacklist
 *
 * Messages with a level other than debug will be sent to the sinks unless the
 * topic is on the blacklist
 *
 * @param s the topic to add
 *
 * @b Example:
 * @code {.cpp}
 * void initialize() {
 *   logger::addWhitelist("lemlib/motions/boomerang");
 *   logger::addWhitelist("lemlib/motions/follow");
 * }
 * @endcode
 */
void addBlacklist(std::string s);

/**
 * @brief remove a topic from the blacklist
 *
 * Messages with a level other than debug will be sent to the sinks unless the
 * topic is on the blacklist
 *
 * @param s the topic to remove
 *
 * @b Example:
 * @code {.cpp}
 * void initialize() {
 *   logger::removeWhitelist("lemlib/motions/boomerang");
 *   logger::removeWhitelist("lemlib/motions/follow");
 * }
 * @endcode
 */
void removeBlacklist(std::string s);

/**
 * @brief Level enum class
 *
 */
enum class Level {
    DEBUG,
    INFO,
    WARN,
    ERROR,
};

/**
 * @brief log a message with a level and topic
 *
 * Messages with a logging level of DEBUG must have the topic whitelisted,
 * while all other messages are sent by default unless the topic is blacklisted
 *
 * @param level the level of the message, e.g INFO
 * @param topic the topic of the message, e.g "lemlib/motions/boomerang"
 * @param format the pre-formatted message, e.g. "Hello, {}!"
 * @param args the arguments to be formatted into the message, e.g. "world"
 *
 * @tparam Args the types of the arguments to be formatted into the message
 */
static void log(Level level, std::string topic, std::string message);

/**
 * @brief Logger Helper class. Used to send messages to all sinks
 *
 */
class Helper {
    public:
        /**
         * @brief Construct a new Helper object
         *
         * @param topic the topic messages will be sent under
         *
         * @b Example:
         * @code {.cpp}
         *   // create a Helper, under the topic doSomething
         *   logger::Helper helper("doSomething");
         *   // log an info message, under te topic doSomething
         *   helper.log(logger::Level::INFO, "Did something!");
         * }
         * @endcode
         */
        Helper(std::string topic);

        /**
         * @brief Send a message to all sinks
         *
         * @param level the logging level of the message
         * @param format the message to be sent
         * @param args the arguments to be formatted into the message
         *
         * @b Example:
         * @code {.cpp}
         * void doSomething() {
         *   // create a Helper
         *   logger::Helper helper("doSomething");
         *   // log an info message
         *   helper.log(logger::Level::INFO, "Motor temperature: {}", 42);
         * }
         * @endcode
         */
        template <typename... Args>
        void log(Level level, const std::string& format, Args&&... args) {
            // format the message into a string using the provided arguments
            std::string message =
                fmt::format(format, std::forward<Args>(args)...);

            logger::log(level, m_topic, message);
        }
    private:
        const std::string m_topic;
};

/**
 * @brief Sink base class
 *
 * A sink is something that can output a message. For example, the controller
 * screen is a sink because you can print strings on it.
 *
 * This abstract class provides a common interface for all sinks.
 */
class Sink {
    public:
        /**
         * @brief Construct a new Sink object
         *
         * This constructor adds the sink to the list of sinks that will be
         * accessed whenever a message is sent.
         *
         * This constructor is automatically called when the constructor
         * of a derived class is called.
         */
        Sink();
        /**
         * @brief Send a message via the sink
         *
         * Derived classes may choose to only send messages with a specific
         * topic or logging level.
         *
         * @param level the logging level of the message
         * @param topic the topic of the message, e.g
         *                  "lemlib/motions/move-to-point"
         * @param message the message to be sent
         */
        virtual void send(Level level, std::string topic,
                          std::string message) = 0;
        /**
         * @brief Destroy the Sink object
         *
         * This destructor removes the sink from the list of sinks that will
         * be accessed whenever a message is sent.
         *
         * This destructor is automatically called when the destructor of a
         * derived class is called.
         *
         * This destructor is marked virtual, so if this destructor is called,
         * the destructor of the derived class will be called first before this
         * destructor continues executing
         */
        virtual ~Sink();
};
} // namespace logger