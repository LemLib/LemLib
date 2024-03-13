#define FMT_HEADER_ONLY
#include "fmt/core.h"
#include "lemlib/logger/buffer.hpp"
#include "lemlib/logger/message.hpp"

namespace lemlib {
Buffer::Buffer(std::function<void(const std::string&)> bufferFunc)
    : bufferFunc(bufferFunc),
      task([=]() { taskLoop(); }) {}

bool Buffer::buffersEmpty() {
    mutex.take();
    bool status = buffer.size() == 0;
    mutex.give();
    return status;
}

Buffer::~Buffer() {
    // make sure when the destructor is called so all
    // the messages are logged
    while (!buffersEmpty()) { pros::delay(10); }
}

void Buffer::pushToBuffer(const std::string& bufferData) {
    mutex.take();
    buffer.push_back(bufferData);
    mutex.give();
}

void Buffer::setRate(uint32_t rate) { this->rate = rate; }

void Buffer::taskLoop() {
    while (true) {
        mutex.take();
        if (buffer.size() > 0) {
            bufferFunc(buffer.at(0));
            buffer.pop_front();
        }
        mutex.give();
        pros::delay(rate);
    }
}
} // namespace lemlib
