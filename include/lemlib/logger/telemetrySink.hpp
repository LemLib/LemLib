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

        /**
         * @brief Set the telemetry id for logging
         *
         * @param id
         */
        void setTelemetryId(uint8_t id);
    protected:
        /**
         * @brief Get the arguments for formatting
         *
         * @return fmt::dynamic_format_arg_store<fmt::format_context>
         */
        fmt::dynamic_format_arg_store<fmt::format_context> getExtraFormattingArgs(const Message& messageInfo) override;
    private:
        /**
         * @brief Log the given message
         *
         * @param message
         */
        void logMessage(const Message& message) override;

        uint8_t id;
};
} // namespace lemlib