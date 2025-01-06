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
 * @brief SlewDirection
 *
 * Slew may only need to be applied when the value being slewed is increasing, decreasing.
 * This enum class help make calls to slew more readable
 */
enum class SlewDirection { INCREASING, DECREASING, ALL };

/**
 * @brief constrain the change in a value over time
 *
 * @param target the requested new value of the changing value
 * @param current the value to be constrained
 * @param maxChange the maximum change allowed per second
 * @param deltaTime the change in time
 * @param directionLimit in which direction to restrict the change. All directions by default
 *
 * @return Number the value with the constrained change
 */
Number slew(Number target, Number current, Number maxChange, Time deltaTime,
            SlewDirection restrictDirection = SlewDirection::ALL);

/**
 * @brief Constrain a value so it's absolute value is greater than some value but less than some other value
 *
 * @param value the value to constrain
 * @param max the maximum absolute value
 * @param min the minimum absolute value
 *
 * @return the constrained value
 *
 * @b Example:
 * @code {.cpp}
 * respectSpeeds(20, 5, 30);  // output: 20
 * respectSpeeds(0, 5, 30);   // output: 5
 * respectSpeeds(40, 5, 30);  // output: 30
 * respectSpeeds(-20, 5, 30); // output: -20
 * respectSpeeds(-2, 5, 30);  // output: -55
 * respectSpeeds(-40, 5, 30); // output: -30
 * @endcode
 */
Number constrainPower(Number power, Number max, Number min);
} // namespace lemlib