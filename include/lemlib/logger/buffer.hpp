#pragma once

#include <deque>
#include <functional>
#include <string>

#include "pros/rtos.hpp"

namespace lemlib {
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
        Buffer(std::function<void(const std::string&)> bufferFunc);

        /**
         * @brief Destroy the Buffer object
         *
         */
        ~Buffer();

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        /**
         * @brief Push to the buffer
         *
         * @param bufferData
         */
        void pushToBuffer(const std::string& bufferData);

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

        std::function<void(std::string)> bufferFunc;

        std::deque<std::string> buffer = {};

        pros::Mutex mutex;
        pros::Task task;

        uint32_t rate;
};
} // namespace lemlib