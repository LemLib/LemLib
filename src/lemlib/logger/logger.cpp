#include "baseSink.hpp"
#include "logger.hpp"

namespace lemlib {
InfoSink& Logger::getInfoSink() {
    static InfoSink infoSink;
    return infoSink;
}

TelemetrySink& Logger::getTelemetrySink() {
    static TelemetrySink telemetrySink;
    return telemetrySink;
}
} // namespace lemlib