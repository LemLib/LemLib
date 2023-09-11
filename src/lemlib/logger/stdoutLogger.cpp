#include "stdoutLogger.hpp"

#include "lemlib/fmt/api.hpp"

std::shared_ptr<lemlib::StdoutLogger> lemlib::StdoutLogger::logger = nullptr;

lemlib::StdoutLogger::StdoutLogger() : task([&]() { loggingTask(); }) {
    setFormat("{color} [LemLib] {severity}: {message}");
}

void lemlib::StdoutLogger::setPrintRate(uint8_t printRate) { this->printRate = printRate; }

std::shared_ptr<lemlib::StdoutLogger> lemlib::StdoutLogger::get() {
    if (logger == nullptr) { logger = std::make_shared<StdoutLogger>(); }

    return logger;
}

void lemlib::StdoutLogger::logString(const lemlib::AbstractLogger::LoggableMessage& message) {
    mutex.take();
    buffer.push_back(message);
    mutex.give();
}

void lemlib::StdoutLogger::loggingTask() {
    while (true) {
        mutex.take();
        if (buffer.size() > 0) {
            lemlib::AbstractLogger::LoggableMessage message = buffer.at(0);
            fmt::print("{}", message.message);
            buffer.pop_front();
        }
        mutex.give();
        pros::delay(printRate);
    }
}