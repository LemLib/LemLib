#include "baseSink.hpp"
#include "logger.hpp"

#include "infoSink.hpp"

namespace lemlib {
std::shared_ptr<InfoSink> Logger::infoSink = nullptr;

InfoSink& getInfoSink() {
    static InfoSink infoSink;
    return infoSink;
}
} // namespace lemlib