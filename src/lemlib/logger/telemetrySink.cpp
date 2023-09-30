#include "telemetrySink.hpp"

#include "stdout.hpp"

namespace lemlib {
TelemetrySink::TelemetrySink() { setFormat("{message}"); }

void TelemetrySink::setTelemetryId(uint8_t id) { this->id = id; }

void TelemetrySink::logMessage(const Message& message) {
    Stdout::print("{}{}\033[2K\r", message.message, static_cast<char>(id));
}
} // namespace lemlib
