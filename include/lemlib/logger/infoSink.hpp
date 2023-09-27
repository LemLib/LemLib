#pragma once

#include "bufferedSink.hpp"
#include "message.hpp"

#include <deque>

#include "pros/rtos.hpp"

namespace lemlib {
/**
 * @brief A sink where info data goes
 *
 */
class InfoSink : public BufferedSink {
    public:
        /**
         * @brief Construct a new Info Sink object
         *
         */
        InfoSink();

        InfoSink(const InfoSink&) = delete;
        InfoSink& operator=(const InfoSink&) = delete;

    protected:
        void handleMessage(const Message& message) override;
};
} // namespace lemlib