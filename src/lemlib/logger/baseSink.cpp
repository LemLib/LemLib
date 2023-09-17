#include "baseSink.hpp"

namespace lemlib {
void BaseSink::setLowestLevel(Level lowestLevel) { this->lowestLevel = lowestLevel; }

void BaseSink::setFormat(const std::string& logFormat) { this->logFormat = logFormat; }
} // namespace lemlib