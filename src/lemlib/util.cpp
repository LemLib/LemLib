/**
 * @file src/lemlib/util.cpp
 * @author Liam Teale
 * @brief File containing definitions for utility functions
 * @version 0.1.0
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <math.h>
#include "lemlib/util.hpp"


/**
 * @brief Slew rate limiter
 * 
 * @param target target value
 * @param current current value
 * @param maxChange maximum change. No maximum if set to 0
 * @return float - the limited value
 */
float lemlib::slew(float target, float current, float maxChange)
{
    float change = target - current;
    if (change > maxChange)
    {
        change = maxChange;
    }
    else if (change < -maxChange)
    {
        change = -maxChange;
    }
    return current + change;
}



/**
 * @brief Convert radians to degrees
 * 
 * @param rad radians
 * @return float degrees
 */
float lemlib::radToDeg(float rad)
{
    return rad * 180 / M_PI;
}


/**
 * @brief Convert degrees to radians
 * 
 * @param deg degrees
 * @return float radians
 */
float lemlib::degToRad(float deg)
{
    return deg * M_PI / 180;
}


/**
 * @brief Calculate the error between 2 angles. Useful when calculating the error between 2 headings
 * 
 * @param angle1
 * @param angle2
 * @param radians true if angle is in radians, false if not. False by default
 * @return float wrapped angle
 */
float lemlib::angleError(float angle1, float angle2, bool radians) {
    float max = radians ? 2 * M_PI : 360;
    float half = radians ? M_PI : 180;
    angle1 = fmod(angle1, max);
    angle2 = fmod(angle2, max);
    float difference = fmin(fabs(angle1 - angle2), max - fabs(angle1 - angle2));
    if (difference != 0 && (angle1 < angle2 ? angle1 - angle2 < half : angle1 - angle2 > half)) {
        difference = -difference;
    }
    return difference;
}


/**
 * @brief Return the sign of a number
 * 
 * @param x the number to get the sign of
 * @return float - -1 if negative, 1 if positive
 */
float sgn(float x)
{
    if (x < 0) return -1; else return 1;
}
