#pragma once
#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include "lemlib/logger/buffer.hpp"
#include "lemlib/logger/message.hpp"

namespace lemlib {

class Message;

enum class Level { Info, Debug, Warning, Error, Fatal };

class Logger {
    public:
        static Logger& getLogger();

        void setLowestLevel(Level level);
        void setFormat(const std::string& format);
        void setTelemetryFormat(const std::string& format);

        void log() {
            Message message(Level::Info, Message::Type::Log, "bob");

            // Buffer::getBuffer().push(Message {Level::Info, Type::Log, "test"});
        }

        void info() {

        };
        void debug();
        void warn();
        void error();
        void fatal();
    private:
        Level lowestLevel = Level::Debug;
        std::string logFormat;
        std::string telemetryFormat;
};

} // namespace lemlib
