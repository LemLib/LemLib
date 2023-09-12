#pragma once

#include "lemlib/logger/abstractSink.hpp"

#include "lemlib/pose.hpp"

#include <string>

namespace lemlib {
/**
 * @brief Format implementation for lemlib::AbstractSink::level
 *
 * @param level The level
 * @return std::string
 */
std::string format_as(AbstractSink::Level level);

/**
 * @brief Formatting implementation for lemlib::Pose
 *
 * @param pose The pose
 * @return std::string
 */
std::string format_as(const Pose& pose);
} // namespace lemlib