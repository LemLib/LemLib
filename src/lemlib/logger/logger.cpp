#include "baseSink.hpp"
#include "logger.hpp"

#include "infoSink.hpp"

namespace lemlib {
InfoSink& Logger::getInfoSink() {
    static InfoSink infoSink;
    return infoSink;
}
} // namespace lemlib