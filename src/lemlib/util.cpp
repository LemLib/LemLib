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
#include "lemlib/pose.hpp"
#include "lemlib/util.hpp"

/**
 * @brief Slew rate limiter
 *
 * @param target target value
 * @param current current value
 * @param maxChange maximum change. No maximum if set to 0
 * @return float - the limited value
 */
float lemlib::slew(float target, float current, float maxChange) {
    float change = target - current;
    if (maxChange == 0) return target;
    if (change > maxChange) change = maxChange;
    else if (change < -maxChange) change = -maxChange;
    return current + change;
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
    return std::remainder(angle1 - angle2, radians ? 2 * M_PI : 360);
}

/**
 * @brief Return the average of a vector of numbers
 *
 * @param values
 * @return float
 */
float lemlib::avg(std::vector<float> values) {
    float sum = 0;
    for (float value : values) { sum += value; }
    return sum / values.size();
}

/**
 * @brief Return the average of a vector of numbers
 *
 * @param values
 * @return double
 */
double lemlib::avg(std::vector<double> values) {
    double sum = 0;
    for (double value : values) { sum += value; }
    return (float)sum / values.size();
}

/**
 * @brief Exponential moving average
 *
 * @param current current measurement
 * @param previous previous output
 * @param smooth smoothing factor (0-1). 1 means no smoothing, 0 means no change
 * @return float - the smoothed output
 */
float lemlib::ema(float current, float previous, float smooth) {
    return (current * smooth) + (previous * (1 - smooth));
}

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
float lemlib::getCurvature(Pose pose, Pose other) {
    // calculate whether the pose is on the left or right side of the circle
    float side = lemlib::sgn(std::sin(pose.theta) * (other.x - pose.x) - std::cos(pose.theta) * (other.y - pose.y));
    // calculate center point and radius
    float a = -std::tan(pose.theta);
    float c = std::tan(pose.theta) * pose.x - pose.y;
    float x = std::fabs(a * other.x + other.y + c) / std::sqrt((a * a) + 1);
    float d = std::hypot(other.x - pose.x, other.y - pose.y);

    // return curvature
    return side * ((2 * x) / (d * d));
}
