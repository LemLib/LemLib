/**
 * @file include/lemlib/util.hpp
 * @author Liam Teale
 * @brief Utility functions declarations
 * @version 0.1.0
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once


#include <initializer_list>
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
    float radToDeg(float rad);

    /**
     * @brief Convert degrees to radians
     * 
     * @param deg degrees
     * @return float radians
     */
    float degToRad(float deg);

    /**
     * @brief Calculate the error between 2 angles. Useful when calculating the error between 2 headings
     * 
     * @param angle1
     * @param angle2
     * @param radians true if angle is in radians, false if not. False by default
     * @return float wrapped angle
     */
    float angleError(float angle1, float angle2, bool radians = false);

    /**
     * @brief Return the sign of a number
     * 
     * @param x the number to get the sign of
     * @return float - -1 if negative, 1 if positive
     */
    float sgn(float x);

    /**
    * @brief Push to the back of an initializer list.
    *
    * @param list the list to push to
    * @param value the value to push
    * @tparam T the type of the list
    *
    * @return std::initializer_list<T> the new list
    */
    template<typename T>
    std::initializer_list<T> push_back(std::initializer_list<T> list, T value);
}
