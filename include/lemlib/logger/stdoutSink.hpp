#pragma once

#include "abstractSink.hpp"

#include <deque>

#include "pros/rtos.hpp"

namespace lemlib {
/**
 * @brief A Sink for stdout.
 *
 */
class StdoutSink : public AbstractSink {
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
        void logString(const AbstractSink::LoggableMessage& message) override;

        static StdoutSink* sink;

        std::deque<AbstractSink::LoggableMessage> buffer = {};

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