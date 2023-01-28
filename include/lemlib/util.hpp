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
}
