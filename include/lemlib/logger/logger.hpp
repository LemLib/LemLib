#pragma once

#include "baseSink.hpp"

#include "infoSink.hpp"
#include "telemetrySink.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include <memory>
#include <array>

namespace lemlib {

/**
 * @brief Get the info sink
 * 
 * @return std::shared_ptr<InfoSink> 
 */
std::shared_ptr<InfoSink> infoSink();

/**
 * @brief Get the telemetry sink
 * 
 * @return std::shared_ptr<TelemetrySink> 
 */
std::shared_ptr<TelemetrySink> telemetrySink();
} // namespace lemlib