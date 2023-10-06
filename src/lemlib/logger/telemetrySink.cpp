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

void TelemetrySink::logMessage(const Message& message) {
    std::string outputEscapeSequence = "\033[2K\r";

    for (char messageChar : message.message) {
        if (messageChar == '\n') {
            outputEscapeSequence += "\033[1A\033[2K\r";
        }
    }
    
    Stdout::print("TELE-BEGIN{}{}TELE-END", message.message, outputEscapeSequence);
}
} // namespace lemlib
