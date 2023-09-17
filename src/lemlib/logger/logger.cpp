#include "logger.hpp"

#include "stdoutSink.hpp"

namespace lemlib {
Logger::Logger(std::initializer_list<BaseSink*> sinks) { this->sinks = sinks; }

Logger Logger::getDefault() { return {StdoutSink::get()}; }
} // namespace lemlib