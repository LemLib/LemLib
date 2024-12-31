#pragma once

#include "units/Angle.hpp"

namespace lemlib {
template <typename T> constexpr T sgn(T val) { return (T(0) < val) - (val < T(0)); }

// TODO: move to appropriate file
// taken from v0.5
/**
 * @brief AngularDirection
 *
 * When turning, the user may want to specify the direction the robot should turn in.
 * This enum class has 3 values: CW_CLOCKWISE, CCW_COUNTERCLOCKWISE, and AUTO
 * AUTO will make the robot turn in the shortest direction, and will be the most used value
 */
enum class AngularDirection { CW_CLOCKWISE, CCW_COUNTERCLOCKWISE, AUTO };

Angle angleError(Angle target, Angle position, AngularDirection direction = AngularDirection::AUTO);
} // namespace lemlib