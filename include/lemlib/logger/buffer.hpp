#pragma once

#include <deque>
#include <functional>
#include <string>

#include "lemlib/logger/message.hpp"
#include "pros/rtos.hpp"

namespace lemlib {
class Message;

/**
 * @brief A buffer implementation
 *
 */
class Buffer {
    public:
        /**
         * @brief Construct a new Buffer object
         *
         */
        Buffer();

        /**
         * @brief Destroy the Buffer object
         *
         */
        ~Buffer();

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        static Buffer& getBuffer();

        /**
         * @brief Push to the buffer
         *
         * @param bufferData
         */
        void push(Message data);

        /**
         * @brief Set the rate of the sink
         *
         * @param rate
         */
        void setRate(uint32_t rate);

        /**
         * @brief Check to see if the buffers are empty
         *
         */
        bool buffersEmpty();
    private:
        /**
         * @brief Code for the logging task
         *
         */
        void loggingTask();

        std::deque<Message> buffer = {};

        pros::Mutex mutex;
        pros::Task task;

        uint32_t rate;
};
} // namespace lemlib
