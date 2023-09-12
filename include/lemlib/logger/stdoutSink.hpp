#pragma once

#include "abstractSink.hpp"

#include <deque>

#include "pros/rtos.hpp"

#include <memory>

namespace lemlib {
/**
 * @brief A Sink for stdout.
 *
 */
class StdoutSink : public AbstractSink {
    public:
        /**
         * @brief Construct a new Stdout Sink object
         *
         */
        StdoutSink();

        StdoutSink(const StdoutSink&) = delete;

        /**
         * @brief Set the print rate of the sink
         *
         * @param printRate The print rate
         */
        void setPrintRate(uint8_t printRate);

        /**
         * @brief Get the stdout sink.
         *
         * @return std::shared_ptr<StdoutSink>
         */
        static std::shared_ptr<StdoutSink> get();
    private:
        /**
         * @brief Log the given string
         *
         * @param message The message to log
         */
        void logString(const AbstractSink::LoggableMessage& message) override;

        static std::shared_ptr<StdoutSink> sink;

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