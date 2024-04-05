#pragma once

#include <cmath>
#include <vector>
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/pose.hpp"

namespace lemlib {
/**
 * @brief Slew rate limiter
 *
 * @param target target value
 * @param current current value
 * @param maxChange maximum change. No maximum if set to 0

 * @return float - the limited value
 */
float slew(float target, float current, float maxChange);

/**
 * @brief Convert radians to degrees
 *
 * @param rad radians
 * @return float degrees
 */
constexpr float radToDeg(float rad) { return rad * 180 / M_PI; }

/**
 * @brief Convert degrees to radians
 *
 * @param deg degrees
 * @return float radians
 */
constexpr float degToRad(float deg) { return deg * M_PI / 180; }

/**
 * @brief Sanitize an angle so its positive and within the range of 0 to 2pi or 0 to 360
 *
 * @param angle the angle to sanitize
 * @param radians whether the angle is in radians or no. True by default
 * @return constexpr float
 */
constexpr float sanitizeAngle(float angle, bool radians = true);

/**
 * @brief Calculate the error between 2 angles. Useful when calculating the error between 2 headings
 *
 * @param target target angle
 * @param position position angle
 * @param radians true if angle is in radians, false if not. False by default
 * @param direction which direction to turn to get to the target angle
 * @return float wrapped angle
 */
float angleError(float target, float position, bool radians = true,
                 AngularDirection direction = AngularDirection::AUTO);

/**
 * @brief Return the sign of a number
 *
 * @param x the number to get the sign of
 * @return int - -1 if negative, 1 if positive
 */
template <typename T> constexpr T sgn(T value) { return value < 0 ? -1 : 1; }

/**
 * @brief Return the average of a vector of numbers
 *
 * @param values
 * @return float
 */
float avg(std::vector<float> values);

/**
 * @brief Exponential moving average
 *
 * @param current current measurement
 * @param previous previous output
 * @param smooth smoothing factor (0-1). 1 means no smoothing, 0 means no change
 * @return float - the smoothed output
 */
float ema(float current, float previous, float smooth);

/**
 * @brief Get the signed curvature of a circle that intersects the first pose and the second pose
 *
 * @note The circle will be tangent to the theta value of the first pose
 * @note The curvature is signed. Positive curvature means the circle is going clockwise, negative means
 * counter-clockwise
 * @note Theta has to be in radians and in standard form. That means 0 is right and increases counter-clockwise
 *
 * @param pose the first pose
 * @param other the second pose
 * @return float curvature
 */
float getCurvature(Pose pose, Pose other);
} // namespace lemlib