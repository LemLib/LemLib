#include "telemetrySink.hpp"

#include "fmt/format.h"
#include "stdout.hpp"

namespace lemlib {
TelemetrySink::TelemetrySink() { setFormat("TELE_{level}:{message}TELE_END"); }

void TelemetrySink::sendMessage(const Message& message) { Stdout::print("\033[s{}\033[u\033[0J", message.message); }
} // namespace lemlib
