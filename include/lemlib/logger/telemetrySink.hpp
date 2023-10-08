#pragma once

#include "baseSink.hpp"

namespace lemlib {
/**
 * @brief A sink where telemetry data goes
 *
 */
class TelemetrySink : public BaseSink {
    public:
        /**
         * @brief Construct a new Telemetry Sink object
         *
         */
        TelemetrySink();
    private:
        /**
         * @brief Log the given message
         *
         * @param message
         */
        void logMessage(const Message& message) override;
};
} // namespace lemlib