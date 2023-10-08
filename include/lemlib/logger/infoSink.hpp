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
    private:
        /**
         * @brief Log the given message
         *
         * @param message
         */
        void sendMessage(const Message& message) override;
};
} // namespace lemlib