#include "logger.hpp"

namespace lemlib {
Logger::Logger(std::initializer_list<std::shared_ptr<AbstractSink>> sinks) { this->sinks = sinks; }
} // namespace lemlib