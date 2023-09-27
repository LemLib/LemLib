#include "baseSink.hpp"
#include "logger.hpp"

#include "infoSink.hpp"

namespace lemlib {
std::shared_ptr<InfoSink> Logger::infoSink = nullptr;

std::shared_ptr<InfoSink> Logger::getInfoSink() {
    if (infoSink == nullptr) {
        infoSink = std::make_shared<InfoSink>();
        infoSink->setFormat("[LemLib] {level}: {message}\n");
    }

    return infoSink;
}
} // namespace lemlib