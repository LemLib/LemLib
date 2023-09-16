#include "logger.hpp"

namespace lemlib {
Logger::Logger(std::initializer_list<BaseSink*> sinks) { this->sinks = sinks; }
} // namespace lemlib