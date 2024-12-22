#pragma once

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
         * @param message the message to send
         *
         * @b Example:
         * @code {.cpp}
         * void doSomething() {
         *   // create a Helper
         *   logger::Helper helper("doSomething");
         *   // log an info message
         *   helper.log(logger::Level::INFO, "Did something!");
         * }
         * @endcode
         */
        void log(Level level, std::string message);
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