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

#include "pose.hpp"
#include "pros/rtos.hpp"
#include "taskwrapper.hpp"

#include <map>
#include <deque>
#include <string>

namespace lemlib {
/*
 * @brief The logging class
 */
class Logger : public TaskWrapper {
    public:
        /*
         * @brief A level enumeration.
         *
         * Debug: Only enabled if lemlib::logger::debug is true
         * Info: General information
         * Warn: Warnings, usually not critical/doesn't affect the robot
         * Error: Errors, usually critical and affects the robot
         * Fatal: Fatal errors, crashes the program
         *
         * @note The log level is inclusive. For example, if the log level is set to INFO, all logs of INFO and higher
         * will be logged.
         */
        enum class Level { DEBUG, INFO, WARN, ERROR, FATAL };
        /*
         * @brief The current lowest log level. Levels below this level will not be logged.
         *
         * @return the lowest loggable level
         */
        Level getLowestLevel();
        /*
         * @brief Sets the lowest loggable level
         *
         * @param level the new lowest loggable level
         */
        void setLowestLevel(Level level);
        /*
         * @brief Sets the format for the output of the log function, as well as its variants for each of the logging
         * levels. All instances of $ and then are placeholders and will be replaced with a value. The default format is
         * "[LemLib] $l: $m". The following placeholders exist:
         * - $t: Time (in milliseconds)
         * - $l: The logging level
         * - $m: The message being logged
         * @param format The new format you want to set.
         */
        void setFormat(const char* format);
        /*
         * @brief Logs a message with an exception
         *
         * @param level the level of the message
         * @param message the message
         * @param exception the exception
         */
        void log(Level level, const char* message, const char* exception);
        /*
         * @brief Logs a message
         *
         * @param level the level of the message
         * @param message the message
         */
        void log(Level level, const char* message);
        /*
         * @brief Logs a debug message
         *
         * @param message
         */
        void debug(const char* message);
        /*
         * @brief Logs an info message
         *
         * @param message
         */
        void info(const char* message);
        /*
         * @brief Logs a warn message
         *
         * @param message
         */
        void warn(const char* message);
        /*
         * @brief Logs an error message
         *
         * @param message
         * @param exception
         */
        void error(const char* message, const char* exception);
        /*
         * @brief Logs an error message
         *
         * @param message
         */
        void error(const char* message);
        /*
         * @brief Logs a fatal message
         *
         * @param message
         * @param exception
         */
        void fatal(const char* message, const char* exception);
        /*
         * @brief Logs a fatal message
         *
         * @param message
         */
        void fatal(const char* message);
        /*
         * @brief Sets the format for the output of the PID logging function, as well as its variants for each of the
         * logging levels. All instances of $ and then are placeholders and will be replaced with a value. The default
         * format is "[LemLib::PID] $n P: $p, I: $i, D: $d". The following placeholders exist:
         * - $t: Time (in milliseconds)
         * - $o: Total ouput
         * - $p: Proportional term
         * - $i: Integral term
         * - $d: Derivative term
         * @param format The new format you want to set.
         */
        void setPidFormat(const char* format);
        /*
         * @brief Logs the output of a PID controller.
         *
         * @param name The name of the controller
         * @param output The total output of the controller which consists of all of the other terms added together.
         * @param p The proportional term of the controller
         * @param i The integral term of the controller
         * @param d The derivative term of the controller
         */
        void logPid(std::string name, float output, float p, float i, float d);
        /*
         * @brief Sets the format for the output of the Odometry logging function, as well as its variants for each of
         * the logging levels. All instances of $ and then are placeholders and will be replaced with a value. The
         * default format is "[LemLib::Odometry] X: $x, Y: $y, Theta: $a". The following placeholders exist:
         * - $t: Time (in milliseconds)
         * - $x: X position
         * - $y: Y Position
         * - $a: angle
         * @param format The new format you want to set.
         */
        void setOdomFormat(const char* format);
        /*
         * @brief Logs the coordinates of a Pose.          *
         * @param currentPose The current position of the robot.
         */
        void logOdom(Pose currentPose);
        /*
         * @brief Sets the speed at which the logger logs messages. The default rate is once every 5ms.
         * @param rate The new print rate in milliseconds.
         */
        void setPrintRate(int rate);
    private:
        pros::Mutex lock;

        Level lowestLevel = Level::INFO;
        bool checkLowestLevel(Logger::Level level);

        int printRate = 5;

        void loop() override;

        std::string logFormat = "[LemLib] $l: $m";
        std::string pidFormat = "[LemLib::PID] $n P: $p, I: $i, D: $d";
        std::string odomFormat = "[LemLib::Odometry] X: $x, Y: $y, Theta: $a";

        std::string formatLog(std::map<std::string, std::string> values, std::string format);

        std::deque<std::string> buffer;
};

inline lemlib::Logger logger; // FIXME: there has got to be a better way to have a global logger class
} // namespace lemlib
