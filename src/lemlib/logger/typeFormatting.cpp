#include "typeFormatting.hpp"

#include "lemlib/fmt/core.h"

std::string lemlib::format_as(lemlib::AbstractLogger::Level level) {
    switch (level) {
        case lemlib::AbstractLogger::Level::DEBUG: return "DEBUG";
        case lemlib::AbstractLogger::Level::INFO: return "INFO";
        case lemlib::AbstractLogger::Level::WARN: return "WARN";
        case lemlib::AbstractLogger::Level::ERROR: return "ERROR";
        case lemlib::AbstractLogger::Level::FATAL: return "FATAL";
    }
    __builtin_unreachable();
}

std::string lemlib::format_as(const Pose& pose) {
    // the double brackets become single brackets
    return fmt::format("lemlib::Pose {{ x: {}, y: {}, theta: {} }}", pose.x, pose.y, pose.theta);
}