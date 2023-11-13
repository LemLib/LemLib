#pragma once

#include <memory>
#include <array>

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include "lemlib/logger/baseSink.hpp"
#include "lemlib/logger/infoSink.hpp"
#include "lemlib/logger/telemetrySink.hpp"

namespace lemlib {
/**
 * @brief Get the info sink.
 * @return std::shared_ptr<InfoSink> A pointer to the info sink
 */
std::shared_ptr<InfoSink> infoSink();

/**
 * @brief Get the telemetry sink.
 * @return std::shared_ptr<TelemetrySink> A pointer to the telemetry
 */
std::shared_ptr<TelemetrySink> telemetrySink();
} // namespace lemlib
