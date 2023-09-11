#pragma once

#include "abstractLogger.hpp"

#include <deque>

#include "pros/rtos.hpp"

#include <memory>

namespace lemlib {
class StdoutLogger : public AbstractLogger {
    public:
        /**
         * @brief Construct a new Stdout Logger object
         *
         */
        StdoutLogger();

        StdoutLogger(const StdoutLogger&) = delete;

        /**
         * @brief Set the print rate of the logger
         *
         * @param printRate The print rate
         */
        void setPrintRate(uint8_t printRate);

        /**
         * @brief Get the stdout logger.
         *
         * @return StdoutLogger*
         */
        static std::shared_ptr<StdoutLogger> get();
    private:
        /**
         * @brief Log the given string
         *
         * @param message The message to log
         */
        void logString(const AbstractLogger::LoggableMessage& message) override;

        static std::shared_ptr<StdoutLogger> logger;

        std::deque<AbstractLogger::LoggableMessage> buffer = {};

        pros::Mutex mutex;
        pros::Task task;

        uint8_t printRate = 5;

        /**
         * @brief Start the logging task.
         *
         */
        void loggingTask();
};

#define LEMLIB_STDOUT_DEBUG(format, ...) ::lemlib::StdoutLogger::get()->debug(format, __VA_ARGS__)
#define LEMLIB_STDOUT_INFO(format, ...) ::lemlib::StdoutLogger::get()->info(format, __VA_ARGS__)
#define LEMLIB_STDOUT_WARN(format, ...) ::lemlib::StdoutLogger::get()->warn(format, __VA_ARGS__)
#define LEMLIB_STDOUT_ERROR(format, ...) ::lemlib::StdoutLogger::get()->error(format, __VA_ARGS__)
#define LEMLIB_STDOUT_FATAL(format, ...) ::lemlib::StdoutLogger::get()->fatal(format, __VA_ARGS__)

} // namespace lemlib