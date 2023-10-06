#pragma once

#include <string>

namespace lemlib {
enum class Level {
    Info,
    Debug,
    Warn,
    Error,
    Fatal,
    Telemetry
}; // This must be defined here, otherwise there will be a circular dependency with the logger. I hate this a
   // lot, but it has to be this way.

class Message {
    public:
        Message(Level i_level, std::string i_message);

        static std::string getLevelString(Level level, bool isColored);

        std::string message;
        Level level = Level::Info;
};
} // namespace lemlib
