/**
 * @file src/lemlib/Logger.cpp
 * @author LemLib Team
 * @brief File containing definitions for the Logger class
 * @version 0.4.5
 * @date 2023-03-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cmath>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <map>
#include <string>

#include "lemlib/logger.hpp"
#include "pros/rtos.hpp"

namespace lemlib {

bool Logger::isEnabled() { return enabled; }

void Logger::setEnabled(bool debug) { debug = debug; }

bool Logger::isVerbose() { return verbose; }

void Logger::setVerbose(bool verbose) { verbose = verbose; }

Logger::Level Logger::getLowestLevel() { return lowestLevel; }

void lemlib::Logger::setLowestLevel(Level level) { lowestLevel = level; }

int ordinal(lemlib::Logger::Level level) { return static_cast<int>(level); }

const std::string RESET_ANSI = "\033[0m";

std::string getColor(Logger::Level level) {
    switch (level) {
        case Logger::Level::DEBUG: return "\033[0;36m"; // cyan
        case Logger::Level::INFO: return "\033[0;32m"; // green
        case Logger::Level::WARN: return "\033[0;33m"; // yellow
        case Logger::Level::ERROR: return "\033[0;31m"; // red
        case Logger::Level::FATAL: return "\033[0;31;2m";
        default: return RESET_ANSI; // reset (white)
    }
}

std::string getFormattedLevel(Logger::Level level) {
    const char* name = "";

    switch (level) {
        case Logger::Level::DEBUG: name = "DEBUG"; break;
        case Logger::Level::INFO: name = "INFO"; break;
        case Logger::Level::WARN: name = "WARN"; break;
        case Logger::Level::ERROR: name = "ERROR"; break;
        case Logger::Level::FATAL: name = "FATAL"; break;
        default: name = "UNKNOWN"; break;
    }

    return getColor(level) + name + RESET_ANSI;
}

bool Logger::checkLowestLevel(Logger::Level level) { return ordinal(level) >= ordinal(lowestLevel); }

std::string Logger::formatLog(std::map<std::string, std::string> values, std::string format) {
    std::string buffer = format;
    size_t startPos = 0;
    while ((startPos = buffer.find("$", startPos)) != std::string::npos) {
        std::string placeholder = buffer.substr(startPos, 2);
        if (values.find(placeholder) != values.end()) {
            buffer.replace(startPos, placeholder.length(), values[placeholder]);
            startPos += values[placeholder].length();
        } else {
            startPos += 1;
        }
    }

    return buffer;
}

void Logger::log(Level level, const char* message) {
    if (!checkLowestLevel(level)) return;
    if (level == Level::DEBUG && !enabled) return;
    if (level == Level::INFO && !verbose) return;

    if (message == nullptr) message = "";

    std::map<std::string, std::string> placeHolderMap;
    placeHolderMap["$m"] = message;
    placeHolderMap["$l"] = getFormattedLevel(level);
    placeHolderMap["$t"] = pros::millis();

    std::string messageString = formatLog(placeHolderMap, logFormat);

    buffer.push_back(messageString);
}

void Logger::log(Level level, const char* message, const char* exception) {
    log(level, message);
    if (message == nullptr) message = "";
    if (exception == nullptr) throw std::invalid_argument("exception cannot be null");
}

void Logger::debug(const char* message) { log(Level::DEBUG, message); }

void Logger::info(const char* message) { log(Level::INFO, message); }

void Logger::warn(const char* message) { log(Level::WARN, message); }

void Logger::error(const char* message, const char* exception) { log(Level::ERROR, message, exception); }

void Logger::error(const char* message) { log(Level::ERROR, message); }

void Logger::fatal(const char* message, const char* exception) { log(Level::FATAL, message, exception); }

void Logger::fatal(const char* message) { log(Level::FATAL, message); }

void Logger::setFormat(const char* format) { logFormat = format; }

void Logger::setPidFormat(const char* format) { pidFormat = format; }

void Logger::setOdomFormat(const char* format) { odomFormat = format; }

void Logger::logPid(std::string name, float output, float p, float i, float d) {
    std::map<std::string, std::string> placeHolderMap;
    placeHolderMap["$n"] = name;
    placeHolderMap["$p"] = p;
    placeHolderMap["$i"] = i;
    placeHolderMap["$d"] = d;
    placeHolderMap["$t"] = pros::millis();
    buffer.push_back(formatLog(placeHolderMap, pidFormat));
}

void Logger::logOdom(Pose currentPose) {
    std::map<std::string, std::string> placeHolderMap;
    placeHolderMap["$x"] = currentPose.x;
    placeHolderMap["$y"] = currentPose.y;
    placeHolderMap["$a"] = currentPose.theta;
    placeHolderMap["$t"] = pros::millis();
    buffer.push_back(formatLog(placeHolderMap, pidFormat));
}

void Logger::loop() {
    while (true) {
        for (auto& message : buffer) { std::cout << message << std::endl; }
        buffer = {};
        pros::delay(20);
    }
}

} // namespace lemlib
