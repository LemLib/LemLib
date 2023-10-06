#include "logger.hpp"
#include "message.hpp"
#include <string>

namespace lemlib {
Level Logger::lowestLevel = Level::Info;
std::string Logger::logFormat = "[LemLib] {level}: {message}";
std::string Logger::telemetryFormat = "TELEMSTART{message}TELEMEND";
bool Logger::isColored = true;

void Logger::setLowestLevel(Level level) { lowestLevel = level; }

void Logger::setFormat(const std::string& format) { logFormat = format; }

void Logger::setTelemetryFormat(const std::string& format) { telemetryFormat = format; }

void Logger::setColored(bool newState) { isColored = newState; }

} // namespace lemlib
