#include "bufferedSink.hpp"
#include "stdoutSink.hpp"

#include "fmt/core.h"
#include "fmt/color.h"

#include "message.hpp"

namespace lemlib {
BufferedSink::BufferedSink() : task([&]() { loggingTask(); }) {}

void BufferedSink::setRate(uint32_t rate) { this->rate = rate; }

void BufferedSink::logMessage(const Message& message) {
    mutex.take();
    buffer.push_back(message);
    mutex.give();
}


void BufferedSink::loggingTask() {
    while (true) {
        mutex.take();
        if (buffer.size() > 0) {
            Message message = buffer.at(0);
            handleMessage(message);
        }
        mutex.give();
        pros::delay(rate);
    }
}
} // namespace lemlib