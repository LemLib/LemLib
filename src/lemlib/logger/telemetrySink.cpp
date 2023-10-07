#include "telemetrySink.hpp"

#include "fmt/format.h"
#include "stdout.hpp"

namespace lemlib {
TelemetrySink::TelemetrySink() { setFormat("TELE_BEGIN{id}{tele_level}{message}TELE_END"); }

void TelemetrySink::setTelemetryId(uint8_t id) { this->id = id; }

fmt::dynamic_format_arg_store<fmt::format_context> TelemetrySink::getExtraFormattingArgs(const Message& messageInfo) {
    fmt::dynamic_format_arg_store<fmt::format_context> args;
    args.push_back(fmt::arg("id", static_cast<char>(id)));
    args.push_back(fmt::arg("tele_level", static_cast<char>(messageInfo.level)));
    return args;
}

void TelemetrySink::logMessage(const Message& message) { Stdout::print("\033[s{}\033[u\033[0J", message.message); }
} // namespace lemlib
