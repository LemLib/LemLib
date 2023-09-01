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
#include "taskwrapper.hpp"

#include <map>
#include <vector>
#include <string>

namespace lemlib {

class Logger : public TaskWrapper {
    public:
        enum class Level { DEBUG, INFO, WARN, ERROR, FATAL };

        bool isEnabled();
        void setEnabled(bool debug);

        bool isVerbose();
        void setVerbose(bool verbose);

        Level getLowestLevel();
        void setLowestLevel(Level level);

        void log(Level level, const char* message, const char* exception);
        void log(Level level, const char* message);
        void debug(const char* message);
        void info(const char* message);
        void warn(const char* message);
        void error(const char* message, const char* exception);
        void error(const char* message);
        void fatal(const char* message, const char* exception);
        void fatal(const char* message);

        // TODO: logging for pure pursuit
        void logPid(std::string name, float output, float p, float i, float d);
        void logOdom(Pose currentPose);

        void setFormat(const char* format);
        void setPidFormat(const char* format);
        void setOdomFormat(const char* format);
    private:
        bool enabled = false;
        bool verbose = false;

        Level lowestLevel = Level::INFO;
        bool checkLowestLevel(Logger::Level level);

        void loop() override;

        std::string logFormat = "[LemLib] $t -- $l: $m";
        std::string pidFormat = "[LemLib::PID] $n P: $p, I: $i, D: $d";
        std::string odomFormat = "[LemLib::Odometry] X: $x, Y: $y, Theta: $a";

        std::string formatLog(std::map<std::string, std::string> values, std::string format);

        std::vector<std::string> buffer;
};

inline lemlib::Logger logger; // FIXME: there has got to be a better way to have a global logger class
} // namespace lemlib
