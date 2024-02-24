#define FMT_HEADER_ONLY
#include "fmt/core.h"
#include "lemlib/logger/buffer.hpp"
#include "lemlib/logger/message.hpp"

namespace lemlib {
Buffer::Buffer(std::function<void(const std::string&)> bufferFunc)
    : bufferFunc(bufferFunc),
      task([this]() { this->taskLoop(); }) {}

bool Buffer::buffersEmpty() {
    this->mutex.take();
    bool status = this->buffer.size() == 0;
    this->mutex.give();
    return status;
}

Buffer::~Buffer() {
    // make sure when the destructor is called so all
    // the messages are logged
    while (!this->buffersEmpty()) { pros::delay(10); }
}

void Buffer::pushToBuffer(const std::string& bufferData) {
    this->mutex.take();
    this->buffer.push_back(bufferData);
    this->mutex.give();
}

void Buffer::setRate(uint32_t rate) { this->rate = rate; }

void Buffer::taskLoop() {
    while (true) {
        this->mutex.take();
        if (buffer.size() > 0) {
            this->bufferFunc(this->buffer.at(0));
            this->buffer.pop_front();
        }
        this->mutex.give();
        pros::delay(this->rate);
    }
}
} // namespace lemlib
