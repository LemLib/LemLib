#include "typeFormatting.hpp"

#include "lemlib/fmt/core.h"

std::string lemlib::format_as(lemlib::AbstractSink::Level level) {
    switch (level) {
        case lemlib::AbstractSink::Level::DEBUG: return "DEBUG";
        case lemlib::AbstractSink::Level::INFO: return "INFO";
        case lemlib::AbstractSink::Level::WARN: return "WARN";
        case lemlib::AbstractSink::Level::ERROR: return "ERROR";
        case lemlib::AbstractSink::Level::FATAL: return "FATAL";
    }
    __builtin_unreachable();
}

std::string lemlib::format_as(const Pose& pose) {
    // the double brackets become single brackets
    return fmt::format("lemlib::Pose {{ x: {}, y: {}, theta: {} }}", pose.x, pose.y, pose.theta);
}