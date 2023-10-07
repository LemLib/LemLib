#include "baseSink.hpp"

namespace lemlib {
void BaseSink::setLowestLevel(Level lowestLevel) { this->lowestLevel = lowestLevel; }

void BaseSink::setFormat(const std::string& logFormat) { this->logFormat = logFormat; }

fmt::dynamic_format_arg_store<fmt::format_context> BaseSink::getExtraFormattingArgs(const Message& messageInfo) {
    return {};
}

void BaseSink::logMessage(const Message& message) {}
} // namespace lemlib