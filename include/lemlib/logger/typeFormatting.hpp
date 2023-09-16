#pragma once

#include <string>

#include "lemlib/pose.hpp"

namespace lemlib {
/**
 * @brief Format a pose
 *
 * @param pose
 * @return std::string
 */
std::string format_as(const Pose& pose);
} // namespace lemlib