/**
 * @file util.cpp
 * @author Liam Teale
 * @brief File containing definitions for utility functions
 * @version 0.1
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "lemlib/util.hpp"


/**
 * @brief Slew rate limiter
 * 
 * @param target target value
 * @param current current value
 * @param maxChange maximum change. No maximum if set to 0
 * @return float - the limited value
 */
float slew(float target, float current, float maxChange)
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
    return (!change) ? target : current + change;
}
