/**
 * @file src/lemlib/util.cpp
 * @author Liam Teale
 * @brief File containing definitions for utility functions
 * @version 0.4.5
 * @date 2023-01-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <math.h>
#include <vector>
#include <memory>
#include <stdexcept>
#include "lemlib/util.hpp"

/**
 * @brief Slew rate limiter
 *
 * @param target target value
 * @param current current value
 * @param maxChange maximum change. No maximum if set to 0
 * @return float - the limited value
 */
float lemlib::util::slew(float target, float current, float maxChange) {
    float change = target - current;
    if (maxChange == 0) return target;
    if (change > maxChange) change = maxChange;
    else if (change < -maxChange) change = -maxChange;
    return current + change;
}

/**
 * @brief Convert radians to degrees
 *
 * @param rad radians
 * @return float degrees
 */
float lemlib::util::radToDeg(float rad) { return rad * 180 / M_PI; }

/**
 * @brief Convert degrees to radians
 *
 * @param deg degrees
 * @return float radians
 */
float lemlib::util::degToRad(float deg) { return deg * M_PI / 180; }

/**
 * @brief Calculate the error between 2 angles. Useful when calculating the error between 2 headings
 *
 * @param angle1
 * @param angle2
 * @param radians true if angle is in radians, false if not. False by default
 * @return float wrapped angle
 */
float lemlib::util::angleError(float angle1, float angle2, bool radians) {
    float max = radians ? 2 * M_PI : 360;
    float half = radians ? M_PI : 180;
    angle1 = fmod(angle1, max);
    angle2 = fmod(angle2, max);
    float error = angle1 - angle2;
    if (error > half) error -= max;
    else if (error < -half) error += max;
    return error;
}

/**
 * @brief Return the sign of a number
 *
 * @param x the number to get the sign of
 * @return float - -1 if negative, 1 if positive
 */
float lemlib::util::sgn(float x) {
    if (x < 0) return -1;
    else return 1;
}
