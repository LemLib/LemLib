/**
 * @file include/lemlib/logger.hpp
 * @author LemLib Team
 * @brief A Logger for LemLib.
 * @version 0.4.5
 * @date 2023-02-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <string>
#include "lemlib/util.hpp"

namespace lemlib {

static bool debug = false;
static bool verbose = false;

namespace logger {

/**
 * @brief A level enumeration.
 *
 * Debug: Only enabled if lemlib::logger::debug is true
 * Info: General information
 * Warn: Warnings, usually not critical/doesn't affect the robot
 * Error: Errors, usually critical and affects the robot
 * Fatal: Fatal errors, crashes the program
 *
 * @note The log level is inclusive. For example, if the log level is set to
 */
enum class Level { DEBUG, INFO, WARN, ERROR, FATAL };

static Level lowestLevel = Level::INFO;

/**
 * @brief Whether or not to log debug messages.
 *
 * @return true if debug is enabled
 */
bool isDebug();
/**
 * @brief Sets lemlib::debug
 *
 * @param debug the new value
 */
void setDebug(bool debug);

/**
 * @brief Whether or not to log info messages.
 *
 * If false, only log messages with a level of lemlib::logger::Level::WARN
 * or higher will be logged
 */
bool isVerbose();
/**
 * @brief Sets lemlib::verbose
 *
 * @param verbose the new value
 */
void setVerbose(bool verbose);

/**
 * @brief The current lowest log level.
 *
 * @return the lowest loggable level
 */
Level getLowestLevel();

/**
 * @brief Sets the lowest loggable level
 *
 * @param level the new lowest loggable level
 */
void setLowestLevel(Level level);

/**
 * @brief Logs a message
 *
 * @param level the level of the message
 * @param message the message
 */
void log(Level level, const std::string& message);

/**
 * @brief Logs a debug message
 *
 * @param message
 */
template <typename... Args>
void debug(const std::string& message, Args const&... args) {
    log(Level::DEBUG, util::format(message, args...));
}

/**
 * @brief Logs an info message
 *
 * @param message
 */
template <typename... Args>
void info(const std::string& message, Args const&... args) {
    log(Level::INFO, util::format(message, args...));
}

/**
 * @brief Logs a warning message
 *
 * @param message
 */
template <typename... Args>
void warn(const std::string& message, Args const&... args) {
    log(Level::WARN, util::format(message, args...));
}

/**
 * @brief Logs an error message
 *
 * @param message
 */
template <typename... Args>
void error(const std::string& message, Args const&... args) {
    log(Level::ERROR, util::format(message, args...));
}

/**
 * @brief Logs a fatal message
 *
 * @param message
 */
template <typename... Args>
void fatal(const std::string& message, Args const&... args) {
    log(Level::FATAL, util::format(message, args...));
}

} // namespace logger
} // namespace lemlib
