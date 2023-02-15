#include <iostream>
#include <string>

#include "lemlib/logger.hpp"


/**
* @brief Whether or not to log debug messages.
*
* @return true if debug is enabled
*/
bool lemlib::logger::isDebug() 
{
    return lemlib::debug;
}

/**
* @brief Sets lemlib::debug
*
* @param debug the new value
*/
void lemlib::logger::setDebug(bool debug) 
{
    lemlib::debug = debug;
}

/**
* @brief Whether or not to log info messages.
*
* If false, only log messages with a level of lemlib::logger::Level::WARN
* or higher will be logged
*/
bool lemlib::logger::isVerbose() 
{
    return lemlib::verbose;
}

/**
* @brief Sets lemlib::verbose
* 
* @param verbose the new value
*/
void lemlib::logger::setVerbose(bool verbose) 
{
    lemlib::verbose = verbose;
}

/**
* @brief The current lowest log level.
*
* @return the lowest loggable level 
*/
lemlib::logger::Level lemlib::logger::getLowestLevel() 
{
    return lemlib::logger::lowestLevel;
}

/**
* @brief Sets the lowest loggable level
* 
* @param level the new lowest loggable level
*/
void lemlib::logger::setLowestLevel(Level level) 
{
    lemlib::logger::lowestLevel = level;
}

/*
Util functions for logger.
Not meant to be used outside of this file.
*/

int ordinal(lemlib::logger::Level level) 
{
    return static_cast<int>(level);
}

const std::string RESET_ANSI = "\033[0m";

std::string getColor(lemlib::logger::Level level)
{
    switch (level) {
        case lemlib::logger::Level::DEBUG:
            return "\033[0;36m"; // cyan
        case lemlib::logger::Level::INFO:
            return "\033[0;32m"; // green
        case lemlib::logger::Level::WARN:
            return "\033[0;33m"; // yellow
        case lemlib::logger::Level::ERROR:
            return "\033[0;31m"; // red
        case lemlib::logger::Level::FATAL:
            return "\033[0;31;2m";
        default:
            return RESET_ANSI; // reset (white)
    }
}

std::string getFormattedLevel(lemlib::logger::Level level) {
    const char* name = "";

    switch (level) {
        case lemlib::logger::Level::DEBUG:
            name = "DEBUG";
            break;
        case lemlib::logger::Level::INFO:
            name = "INFO";
            break;
        case lemlib::logger::Level::WARN:
            name = "WARN";
            break;
        case lemlib::logger::Level::ERROR:
            name = "ERROR";
            break;
        case lemlib::logger::Level::FATAL:
            name = "FATAL";
            break;
        default:
            name = "UNKNOWN";
            break;
    }

    return getColor(level) + name;
}

bool checkLowestLevel(lemlib::logger::Level level) 
{
    return ordinal(level) >= ordinal(lemlib::logger::lowestLevel);
}



/*
End of util functions
*/

/**
    * @brief Logs a message with an exception
    * 
    * @param level the level of the message
    * @param message the message
    * @param exception the exception
    */
void lemlib::logger::log(Level level, std::string message, std::string exception) 
{
    if (!checkLowestLevel(level)) return;
    if (level == Level::DEBUG && !lemlib::debug) return;
    if (level == Level::INFO && !lemlib::verbose) return;

    std::string messageString = "[LemLib] " + getFormattedLevel(level) +  ": " + message + ": " + exception + RESET_ANSI;

    std::cout << messageString << std::endl;
}

/**
    * @brief Logs a message
    * 
    * @param level the level of the message
    * @param message the message
    */
void lemlib::logger::log(Level level, std::string message) 
{
    if (!checkLowestLevel(level)) return;
    if (level == Level::DEBUG && !lemlib::debug) return;
    if (level == Level::INFO && !lemlib::verbose) return;

    std::string messageString = "[LemLib] " + getFormattedLevel(level) +  ": " + message + RESET_ANSI;

    std::cout << messageString << std::endl;
}

/**
    * @brief Logs a debug message
    * 
    * @param message 
    */
void lemlib::logger::debug(std::string message) 
{
    log(Level::DEBUG, message);
}

/**
    * @brief Logs an info message
    * 
    * @param message 
    */
void lemlib::logger::info(std::string message) 
{
    log(Level::INFO, message);
}

/**
    * @brief Logs a warning message
    * 
    * @param message 
    */ 
void lemlib::logger::warn(std::string message) 
{
    log(Level::WARN, message);
}

/**
    * @brief Logs an error message
    * 
    * @param message
    * @param exception
    */
void lemlib::logger::error(std::string message, std::string exception) 
{
    log(Level::ERROR, message, exception);
}

/**
    * @brief Logs an error message
    * 
    * @param message 
    */
void lemlib::logger::error(std::string message) 
{
    log(Level::ERROR, message);
}

/**
    * @brief Logs a fatal message
    * 
    * @param message 
    * @param exception 
    */
void lemlib::logger::fatal(std::string message, std::string exception) 
{
    log(Level::FATAL, message, exception);
}

/**
    * @brief Logs a fatal message
    * 
    * @param message 
    */
void lemlib::logger::fatal(std::string message) 
{
    log(Level::FATAL, message);
}