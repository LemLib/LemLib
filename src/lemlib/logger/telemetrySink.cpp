#include "telemetrySink.hpp"

#include "fmt/format.h"
#include "stdout.hpp"

namespace lemlib {
TelemetrySink::TelemetrySink() { setFormat("{id}{message}"); }

void TelemetrySink::setTelemetryId(uint8_t id) { this->id = id; }

fmt::dynamic_format_arg_store<fmt::format_context> TelemetrySink::getExtraFormattingArgs() {
    fmt::dynamic_format_arg_store<fmt::format_context> args;
    args.push_back(fmt::arg("id", id));
    return args;
}

void TelemetrySink::logMessage(const Message& message) { Stdout::print("{}\033[2K\r", message.message); }
} // namespace lemlib
