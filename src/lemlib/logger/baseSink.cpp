#include "lemlib/logger/baseSink.hpp"

namespace lemlib {
BaseSink::BaseSink(std::initializer_list<std::shared_ptr<BaseSink>> sinks) { this->sinks = sinks; }

void BaseSink::setLowestLevel(Level lowestLevel) {
    if (!sinks.empty()) {
        for (std::shared_ptr<BaseSink> sink : sinks) { sink->setLowestLevel(lowestLevel); }
        return;
    }

    this->lowestLevel = lowestLevel;
}

void BaseSink::setFormat(const std::string& logFormat) { this->logFormat = logFormat; }

fmt::dynamic_format_arg_store<fmt::format_context> BaseSink::getExtraFormattingArgs(const Message& messageInfo) {
    return {};
}

void BaseSink::sendMessage(const Message& message) {}
} // namespace lemlib