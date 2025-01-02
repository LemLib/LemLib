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

/**
 * @brief Calculate the error between two angles
 *
 * @param target the target angle
 * @param position the current angle
 * @param direction which direction the robot should turn in. Defaults to AUTO
 * @return Angle the error between the two angles
 */
Angle angleError(Angle target, Angle position, AngularDirection direction = AngularDirection::AUTO);
/**
 * @brief Applies a slew rate limit to a value. Slew prevents the value from accelerating too quickly
 *
 * @param target the target value
 * @param current the current value
 * @param maxChange the maximum change in the value
 * @return double the new value
 */
double slew(double target, double current, double maxChange);
/**
 * @brief Respects a motor power to it's maximum and minimum speeds, as well as an optional slew rate.
 *
 * @param power the power to respect
 * @param previous the previous power
 * @param max the maximum power
 * @param min the minimum power
 * @param slew the slew rate. If set to 0, no slew rate will be applied
 * @return double the respected power
 */
double respectSpeeds(double power, double previous, double max, double min, double slew);
} // namespace lemlib