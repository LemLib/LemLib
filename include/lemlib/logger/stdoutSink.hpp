#pragma once

#include "baseSink.hpp"
#include "message.hpp"

#include <deque>

#include "pros/rtos.hpp"

namespace lemlib {
/**
 * @brief Implementation of the Stdout sink
 *
 */
class StdoutSink : public BaseSink {
    public:
        StdoutSink(const StdoutSink&) = delete;
        StdoutSink& operator=(const StdoutSink&) = delete;

        /**
         * @brief Set the print rate of the sink
         *
         * @param printRate The print rate
         */
        void setPrintRate(uint8_t printRate);

        /**
         * @brief Get the stdout sink.
         *
         * @return StdoutSink*
         */
        static StdoutSink* get();
    private:
        /**
         * @brief Construct a new Stdout Sink object
         *
         */
        StdoutSink();

        /**
         * @brief Log the given string
         *
         * @param message The message to log
         */
        void logMessage(const Message& message) override;

        static StdoutSink* sink;

        std::deque<Message> buffer = {};

        pros::Mutex mutex;
        pros::Task task;

        uint8_t printRate = 5;

        /**
         * @brief Start the logging task.
         *
         */
        void loggingTask();
};
} // namespace lemlib