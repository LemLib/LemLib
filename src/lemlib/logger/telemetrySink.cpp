#include "telemetrySink.hpp"

#include "fmt/format.h"
#include "stdout.hpp"

namespace lemlib {
TelemetrySink::TelemetrySink() { setFormat("TELE-BEGIN{id}{tele_level}{message}TELE-END"); }

void TelemetrySink::setTelemetryId(uint8_t id) { this->id = id; }

fmt::dynamic_format_arg_store<fmt::format_context> TelemetrySink::getExtraFormattingArgs(const Message& messageInfo) {
    fmt::dynamic_format_arg_store<fmt::format_context> args;
    args.push_back(fmt::arg("id", id));
    args.push_back(fmt::arg("tele_level", static_cast<uint8_t>(messageInfo.level)));
    return args;
}

void TelemetrySink::logMessage(const Message& message) { Stdout::print("\033[s{}\033[u\033[0J", message.message); }
} // namespace lemlib
