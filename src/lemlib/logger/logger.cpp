#include "logger.hpp"

namespace lemlib {
Logger::Logger(std::initializer_list<AbstractSink*> sinks) { this->sinks = sinks; }
} // namespace lemlib