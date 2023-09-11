#include "abstractLogger.hpp"

void lemlib::AbstractLogger::setLowestLevel(lemlib::AbstractLogger::Level lowestLevel) {
    this->lowestLevel = lowestLevel;
}

std::string format_as(lemlib::AbstractLogger::Level level) {
    switch (level) {
        case lemlib::AbstractLogger::Level::DEBUG: return "DEBUG";
        case lemlib::AbstractLogger::Level::INFO: return "INFO";
        case lemlib::AbstractLogger::Level::WARN: return "WARN";
        case lemlib::AbstractLogger::Level::ERROR: return "ERROR";
        case lemlib::AbstractLogger::Level::FATAL: return "FATAL";
    }
    __builtin_unreachable();
}
