#include <vector>
#include <string.h>
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
 * Finds the curvature of a circle which intersects 2 points, and is tangent to the first point
 *
 * Inspired by: https://www.chiefdelphi.com/t/paper-implementation-of-the-adaptive-pure-pursuit-controller/166552
 */
float lemlib::getCurvature(Pose p1, Pose p2) {
    // calculate whether the pose is on the left or right side of the circle
    float side = sgn(std::sin(p1.theta) * (p2.x - p1.x) - std::cos(p1.theta) * (p2.y - p1.y));
    // calculate center point and radius
    float a = -std::tan(p1.theta);
    float c = std::tan(p1.theta) * p1.x - p1.y;
    float x = std::fabs(a * p2.x + p2.y + c) / std::sqrt((a * a) + 1);
    float d = p1.distance(p2);

    // return curvature
    return side * ((2 * x) / (d * d));
}