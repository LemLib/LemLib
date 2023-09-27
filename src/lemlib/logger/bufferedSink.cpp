#include "bufferedSink.hpp"

#include "fmt/core.h"
#include "fmt/color.h"

#include "message.hpp"

namespace lemlib {
BufferedSink::BufferedSink() : task([&]() { loggingTask(); }) {}

bool BufferedSink::buffersEmpty() {
    mutex.take();
    bool status = buffer.size() == 0;
    mutex.give();
    return status;
}

BufferedSink::~BufferedSink() {
    // make sure when the destructor is called so all
    // the messages are logged
    while (!buffersEmpty()) { pros::delay(10); }
}

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
            buffer.pop_front();
        }
        mutex.give();
        pros::delay(rate);
    }
}
} // namespace lemlib