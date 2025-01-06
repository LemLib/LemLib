#pragma once

#include "units/Angle.hpp"
#include "units/Pose.hpp"

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
/**
 * @brief Get the signed curvature of a circle that intersects the first pose and the second pose
 *
 * This is a very niche function that is only used in Pure Pursuit and Boomerang. It calculates the curvature of a
 * circle that is tangent to the first pose and intersects the second pose. It's also signed to indicate whether the
 * robot should turn clockwise or counter-clockwise to get to the second pose
 *
 * @note The circle will be tangent to the theta value of the first pose
 * @note The curvature is signed. Positive curvature means the circle is going clockwise, negative means
 * counter-clockwise
 * @note Theta has to be in radians and in standard form. That means 0 is right and increases counter-clockwise
 *
 * @param pose the first pose
 * @param other the second pose
 * @return float curvature
 *
 * @b Example
 * @code {.cpp}
 * Pose pose = {0, 0, 0};
 * Pose other = {0, 10, 0};
 * double curvature = getCurvature(pose, other);
 * @endcode
 */
double getCurvature(units::Pose pose, units::Pose other);
/**
 * @brief Ratio the speeds to respect the max. This is important because if the robot is moving at maximum speed with a
 * desired non-zero angular velocity, it will be unable to turn, so we ratio the by the max speed so that it will move
 * at the maximum speed that it can properly turn at.
 *
 * @param lateral the lateral speed
 * @param angular the angular speed
 * @param maxSpeed the maximum speed
 * @return std::tuple<double, double> the ratio-d speeds
 */
std::tuple<double, double> ratioSpeeds(double lateral, double angular, double maxSpeed);
} // namespace lemlib