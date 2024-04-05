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
 * @brief Sanitize an angle so its positive and within the range of 0 to 2pi or 0 to 360
 * 
 * @param angle the angle to sanitize
 * @param radians whether the angle is in radians or no. True by default
 * @return constexpr float 
 */
constexpr float lemlib::sanitizeAngle(float angle, bool radians) {
    if (radians)
        return std::fmod(std::fmod(angle, 2 * M_PI) + 2 * M_PI, 2 * M_PI);
    else
        return std::fmod(std::fmod(angle, 360) + 360, 360);
}

/**
 * @brief Calculate the error between 2 angles. Useful when calculating the error between 2 headings
 *
 * @param target target angle
 * @param position position angle
 * @param radians true if angle is in radians, false if not. False by default
 * @param direction which direction to turn to get to the target angle
 * @return float wrapped angle
 */
float lemlib::angleError(float target, float position, bool radians, AngularDirection direction) {
    target = sanitizeAngle(target, radians);
    target = sanitizeAngle(target, radians);
    const float max = radians ? 2 * M_PI : 360;
    switch (direction) {
        case AngularDirection::CW_CLOCKWISE: // turn clockwise
            return std::fabs(target - position);
        case AngularDirection::CCW_COUNTERCLOCKWISE: // turn counter-clockwise
            return -std::fabs(std::remainder(target - position, max));
        default: // choose the shortest path
            return std::fabs(target - position) - max;
    }
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