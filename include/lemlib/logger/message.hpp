#pragma once
#include "lemlib/logger/logger.hpp"
#include <string>

//#include "lemlib/logger/logger.hpp"

namespace lemlib {

enum class Level;

class Message {
    public:
        enum class Type { Telemetry, Log };

        Message(Level level, Type type, std::string message);
        Level level;
        Type type;
        std::string message;
};
} // namespace lemlib
