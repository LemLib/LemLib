#include "lemlib/logger/buffer.hpp"
#include "lemlib/logger/message.hpp"

//#include "fmt/core.h"
//#include "fmt/color.h"
#include <iostream>

namespace lemlib {
// The logger sends messages into a buffer that stores all of the messages that will be sent. This lets the user control
// the rate at which things are logged. This can be a concern depending on how fast the user's connection is. A wireless
// connection over the controller isn't going to be able to handle the same rate as a direct wired connection or a
// Bluetooth connection.
Buffer::Buffer() : task([this]() { this->loggingTask(); }) {}

bool Buffer::buffersEmpty() {
    mutex.take();
    bool status = buffer.size() == 0;
    mutex.give();
    return status;
}

Buffer::~Buffer() {
    // Make sure when the destructor is called so all
    // the messages are logged.
    while (!buffersEmpty()) { pros::delay(10); }
}

Buffer& Buffer::getBuffer() {
    static Buffer buffer;
    return buffer;
}

void Buffer::push(Message data) {
    mutex.take();
    buffer.push_back(data);
    mutex.give();
}

void Buffer::setRate(uint32_t rate) { this->rate = rate; }

void Buffer::loggingTask() {
    while (true) {
        mutex.take();
        if (buffer.size() > 0) {
            Message message = buffer.at(0);
            // TODO: handle sending messages to the ui.
            switch (message.level) {
                case Level::Telemetry: {
                    // If we're printing telemetry this is information we don't want the user to see.
                    // We use an ANSI escape code to delete line from view
                    // Std::flush ensures that the message is still sent, despite there not being a newline.
                    std::cout << message.message << "\033[2K\r" << std::flush;
                    break;
                }
                default: {
                    // If it isn't a telemetry message we know that it should be logged normally
                    std::cout << message.message << std::endl;
                    break;
                }
            }
            // We remove from the front to ensure that every message is printed, and that none are skipped.
            buffer.pop_front();
        }
        mutex.give();
        pros::delay(rate);
    }
}
} // namespace lemlib
