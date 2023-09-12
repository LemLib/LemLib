#include "stdoutSink.hpp"

#include "lemlib/fmt/api.hpp"

lemlib::StdoutSink* lemlib::StdoutSink::sink = nullptr;

lemlib::StdoutSink::StdoutSink() : task([&]() { loggingTask(); }) {
    setFormat("{color} [LemLib] {severity}: {message}");
}

void lemlib::StdoutSink::setPrintRate(uint8_t printRate) { this->printRate = printRate; }

lemlib::StdoutSink* lemlib::StdoutSink::get() {
    if (sink == nullptr) { sink = new StdoutSink; }

    return sink;
}

void lemlib::StdoutSink::logString(const lemlib::AbstractSink::LoggableMessage& message) {
    mutex.take();
    buffer.push_back(message);
    mutex.give();
}

void lemlib::StdoutSink::loggingTask() {
    while (true) {
        mutex.take();
        if (buffer.size() > 0) {
            lemlib::AbstractSink::LoggableMessage message = buffer.at(0);
            fmt::print("{}", message.message);
            buffer.pop_front();
        }
        mutex.give();
        pros::delay(printRate);
    }
}