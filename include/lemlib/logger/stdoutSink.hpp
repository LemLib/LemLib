#pragma once

#include "bufferedSink.hpp"
#include "message.hpp"

#include <deque>

#include "pros/rtos.hpp"

namespace lemlib {
/**
 * @brief Implementation of the Stdout sink
 *
 */
class StdoutSink : public BufferedSink {
    public:
        /**
         * @brief Construct a new Stdout Sink object
         *
         */
        StdoutSink();

        StdoutSink(const StdoutSink&) = delete;
        StdoutSink& operator=(const StdoutSink&) = delete;

        /**
         * @brief Set the color mode (true enables, false disables)
         *
         * @param mode
         */
        void flipColorMode(bool mode);

        /**
         * @brief Get the stdout sink.
         *
         * @return StdoutSink*
         */
        static StdoutSink* get();
    protected:
        void handleMessage(const Message& message) override;
    private:
        static StdoutSink* sink;

        bool colorMode = true;
};
} // namespace lemlib