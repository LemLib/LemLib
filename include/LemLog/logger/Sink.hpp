#pragma once

#include <string>
#include <list>

namespace logger {

enum class Level {
    DEBUG,
    INFO,
    WARN,
    ERROR,
};

/**
 * @brief send a message to all sinks
 *
 * @param level the logging level of the message
 * @param topic the topic of the message, e.g
 *                  "lemlib/motions/move-to-point"
 * @param message the message to be sent
 */
void log(Level level, const std::string& topic, const std::string& message);

enum class SinkStatus {
    OK, /** sink write OK, no action necessary */
    WARNING, /** sink write warning, message to all other sinks */
    ERROR, /** sink write error, message remaining sinks and remove sink */
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
        friend void log(Level level, const std::string& topic,
                        const std::string& message);
    public:
        /**
         * @brief Construct a new Sink object
         *
         * This constructor adds the sink to the list of sinks that will be
         * accessed whenever a message is sent.
         *
         * This constructor is automatically called when the constructor
         * of a derived class is called.
         *
         * @param name the name of the sink
         */
        Sink(std::string name);
        /**
         * @brief add a topic to the allowlist
         *
         * @param topic the topic to add to the allowlist
         */
        void addToAllowList(const std::string& topic);
        /**
         * @brief remove a topic from the allowlist
         *
         * @param topic the topic to remove from the allowlist
         */
        void removeFromAllowList(const std::string& topic);
        /**
         * @brief add a topic to the blocked list
         *
         * @param topic the topic to add to the blocked list
         */
        void addToBlockedList(const std::string& topic);
        /**
         * @brief remove a topic from the blocked list
         *
         * @param topic the topic to remove from the blocked list
         */
        void removeFromBlockedList(const std::string& topic);
        /**
         * @brief Set the lowest level that will be logged
         *
         * @param level the lowest level that will be logged
         */
        void setLoggingLevel(Level level);
        /**
         * @brief Get the name of the sink
         *
         * @return const std::string& const reference to the name of the
         * sink
         */
        const std::string& getName() const&;
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
    protected:
        /**
         * @brief Write a message to the sink
         *
         * This function should be implemented
         *
         * @param level the logging level of the message
         * @param topic the topic of the message, e.g
         *                  "lemlib/motions/move-to-point"
         * @param message the message to be sent
         *
         * @return SinkStatus the status of the sink. OK, WARNING, or ERROR
         */
        virtual SinkStatus write(Level level, const std::string& topic,
                                 const std::string& message) = 0;
    private:
        /**
         * @brief send a message to the sink
         *
         * This function is called by the logger. It filters messages before
         * calling the write member function. This is to simplify the
         * implementation of custom sinks as well as enforce the use of the
         * allowlist and blockedlist. It also enforces the use of minimum
         * logging levels
         *
         * @param level the logging level of the message
         * @param topic the topic of the message, e.g
         *                  "lemlib/motions/move-to-point"
         * @param message the message to be sent
         *
         * @return SinkStatus the status of the sink. OK, WARNING, or ERROR
         */
        SinkStatus send(Level level, const std::string& topic,
                        const std::string& message);
        const std::string m_name;
        Level m_minLevel = Level::INFO;
        std::list<std::string> m_allowList;
        std::list<std::string> m_blockedList;
};
} // namespace logger