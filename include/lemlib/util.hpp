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
 *
 * @b Example
 * @code {.cpp}
 * float limited = slew(100, // target value
 *                      0, // current value
 *                      10); // maximum allowed change
 * // limited == 10
 * float limited2 = slew(4, // target value
 *                       0, // current value
 *                       10); // maximum allowed change
 * // limited2 == 4
 * @endcode
 */
float slew(float target, float current, float maxChange);

/**
 * @brief Convert radians to degrees
 *
 * @param rad radians
 * @return float degrees
 *
 * @b Example
 * @code {.cpp}
 * radToDeg(M_PI); // returns 180
 * @endcode
 */
constexpr float radToDeg(float rad) { return rad * 180 / M_PI; }

/**
 * @brief Convert degrees to radians
 *
 * @param deg degrees
 * @return float radians
 *
 * @b Example
 * @code {.cpp}
 * degToRad(180); // returns 3.14159... (pi)
 * @endcode
 */
constexpr float degToRad(float deg) { return deg * M_PI / 180; }

/**
 * @brief Sanitize an angle so its positive and within the range of 0 to 2pi or 0 to 360
 *
 * @param angle the angle to sanitize
 * @param radians whether the angle is in radians or no. True by default
 * @return constexpr float
 *
 * @b Example
 * @code {.cpp}
 * // sanitize angle in degrees
 * sanitizeAngle(-90, false); // returns 270
 * sanitizeAngle(370, false); // returns 10
 * // sanitize angle in radians
 * sanitizeAngle(-M_PI, true); // returns pi
 * sanitizeAngle(7 * M_PI, true); // returns pi
 * // you can also use the default value of radians
 * sanitizeAngle(-M_PI); // returns pi
 * sanitizeAngle(7 * M_PI); // returns pi
 * @endcode
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
 *
 * @b Example
 * @code {.cpp}
 * angleError(10, 350, false); // returns 20
 * angleError(350, 10, false); // returns -20
 * @endcode
 */
float angleError(float target, float position, bool radians = true,
                 AngularDirection direction = AngularDirection::AUTO);

/**
 * @brief Return the sign of a number
 *
 * @param x the number to get the sign of
 * @return int - -1 if negative, 1 if positive
 *
 * @b Example
 * @code {.cpp}
 * sgn(-10); // returns -1
 * sgn(10); // returns 1
 * sgn(0); // returns 1 (by convention)
 * @endcode
 */
template <typename T> constexpr T sgn(T value) { return value < 0 ? -1 : 1; }

/**
 * @brief Return the average of a vector of numbers
 *
 * @param values
 * @return float
 *
 * @b Example
 * @code {.cpp}
 * std::vector<float> values = {1, 2, 3, 4, 5};
 * avg(values); // returns 3
 * @endcode
 */
float avg(std::vector<float> values);

/**
 * @brief Exponential moving average
 *
 * @param current current measurement
 * @param previous previous output
 * @param smooth smoothing factor (0-1). 1 means no smoothing, 0 means no change
 * @return float - the smoothed output
 *
 * @b Example
 * @code {.cpp}
 * ema(10, 0, 0.5); // returns 5
 * @endcode
 */
float ema(float current, float previous, float smooth);

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
 * float curvature = getCurvature(pose, other);
 * @endcode
 */
float getCurvature(Pose pose, Pose other);
} // namespace lemlib