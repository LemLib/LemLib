#pragma once

#include "lemlib/logger/abstractLogger.hpp"

#include "lemlib/pose.hpp"

#include <string>

namespace lemlib {
/**
 * @brief Format implementation for lemlib::AbstractLogger::level
 *
 * @param level The level
 * @return std::string
 */
std::string format_as(AbstractLogger::Level level);

/**
 * @brief Formatting implementation for lemlib::Pose
 *
 * @param pose The pose
 * @return std::string
 */
std::string format_as(const Pose& pose);
} // namespace lemlib