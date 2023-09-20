#pragma once

#include "baseSink.hpp"
#include "message.hpp"

#include <deque>
#include <functional>

#include "pros/rtos.hpp"


namespace lemlib {
/**
 * @brief A buffered sink
 *
 */
class BufferedSink : public BaseSink {
    public:
        /**
         * @brief Construct a new Buffered Sink object
         *
         */
        BufferedSink();

        BufferedSink(const BufferedSink&) = delete;
        BufferedSink& operator=(const BufferedSink&) = delete;

        /**
         * @brief Set the rate of the sink
         *
         * @param rate
         */
        void setRate(uint32_t rate);
    protected:
        /**
         * @brief Handle a message that's been popped of the buffer
         *
         * @param message
         */
        virtual void handleMessage(const Message& message) = 0;
    private:
        /**
         * @brief Log the given message
         *
         * @param message
         */
        void logMessage(const Message& message) override;

        std::deque<Message> buffer = {};

        pros::Mutex mutex;
        pros::Task task;

        uint32_t rate;

        /**
         * @brief Start the task.
         *
         */
        void loggingTask();
};
} // namespace lemlib