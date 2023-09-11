#include "stdoutLogger.hpp"

#include "lemlib/fmt/api.hpp"

lemlib::StdoutLogger* lemlib::StdoutLogger::logger = nullptr;

lemlib::StdoutLogger::StdoutLogger() : task([&]() { loggingTask(); }) {}

void lemlib::StdoutLogger::setPrintRate(uint8_t printRate) { this->printRate = printRate; }

lemlib::StdoutLogger* lemlib::StdoutLogger::get() {
    if (logger == nullptr) { logger = new lemlib::StdoutLogger(); }

    return logger;
}

void lemlib::StdoutLogger::logString(const lemlib::AbstractLogger::LoggableMessage& message) {
    mutex.take();
    buffer.push_back(message);
    mutex.give();
}

static fmt::color getColorFromLevel(lemlib::AbstractLogger::Level level) {
    switch (level) {
        case lemlib::AbstractLogger::Level::DEBUG: return fmt::color::cyan;
        case lemlib::AbstractLogger::Level::INFO: return fmt::color::green;
        case lemlib::AbstractLogger::Level::WARN: return fmt::color::yellow;
        case lemlib::AbstractLogger::Level::ERROR: return fmt::color::red;
        case lemlib::AbstractLogger::Level::FATAL: return fmt::color::indian_red;
    }
    __builtin_unreachable();
}

void lemlib::StdoutLogger::loggingTask() {
    while (true) {
        mutex.take();
        if (buffer.size() > 0) {
            lemlib::AbstractLogger::LoggableMessage message = buffer.at(0);
            fmt::color color = getColorFromLevel(message.level);
            fmt::print(fmt::bg(color), "{}", message.message);
            buffer.pop_front();
        }
        mutex.give();
        pros::delay(printRate);
    }
}