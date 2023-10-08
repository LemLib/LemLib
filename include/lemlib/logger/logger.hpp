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
 * @brief Get the info sink.
 * This is the primary way of interacting with LemLib's logging implementation. This sink is used for printing useful
 * information to the user's terminal. For more information see InfoSink.
 * @b Example Usage
 * @code
 * lemlib::infoSink()->setLowestLevel(lemlib::Level::INFO);
 * lemlib::infoSink()->info("hello {}!", "world");
 * lemlib::infoSink()->debug("debug info: {}!", "my cool info here");
 * @endcode
 * @return std::shared_ptr<InfoSink>
 */
std::shared_ptr<InfoSink> infoSink();

/**
 * @brief Get the telemetry sink.
 * This is the primary way of interacting with the telemetry portion of LemLib's logging implementation. This sink is
 * used for sending data that is not meant to be viewed by the user, but will still be used by something else, like a
 * data visualization tool. Messages sent through this sink will not be cleared from the terminal and not be visible to
 * the user. For more information see TelemetrySink.
 * @b Example Usage
 * @code
 * #define TEMPERATURE_ID 1
 * lemlib::telemetrySink()->setLowestLevel(lemlib::Level::INFO);
 * lemlib::telemetrySink()->setLowestLevel(lemlib::Level::INFO);
 * lemlib::telemetrySink()->info("{},{},{}", TEMPERATURE_ID, motor1.get_temperature(), motor2.get_temperature());
 * @endcode
 * @return std::shared_ptr<TelemetrySink>
 */
std::shared_ptr<TelemetrySink> telemetrySink();
} // namespace lemlib
