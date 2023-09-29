#pragma once

#include "message.hpp"
#include "baseSink.hpp"

#include <deque>

#include "pros/rtos.hpp"

namespace lemlib {
/**
 * @brief A sink where info data goes
 *
 */
class InfoSink : public BaseSink {
    public:
        /**
         * @brief Construct a new Info Sink object
         *
         */
        InfoSink();

        InfoSink(const InfoSink&) = delete;
        InfoSink& operator=(const InfoSink&) = delete;
    private:
        /**
         * @brief Log the given message
         *
         * @param message
         */
        void logMessage(const Message& message) override;
};
} // namespace lemlib