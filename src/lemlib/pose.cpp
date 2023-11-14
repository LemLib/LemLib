/**
 * @file src/lemlib/pose.cpp
 * @author LemLib Team
 * @brief Source file containing the implementation of the Pose class
 * @version 0.4.5
 * @date 2023-01-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <math.h>

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include "lemlib/pose.hpp"

namespace lemlib {
/**
 * @brief Create a new pose
 *
 * @param x component
 * @param y component
 * @param theta heading. Defaults to 0
 */
Pose::Pose(Length x, Length y, Angle theta)
    : x(x),
      y(y),
      theta(theta) {}

/**
 * @brief Add a pose to this pose
 *
 * @param other other pose
 * @return Pose
 */
Pose Pose::operator+(const Pose& other) { return Pose(x + other.x, y + other.y, theta); }

/**
 * @brief Set the value of this pose to its value + the value of another pose
 *
 * @param other other pose
 */
void Pose::operator+=(const Pose& other) {
    x += other.x;
    y += other.y;
    theta += other.theta;
}

/**
 * @brief Subtract a pose from this pose
 *
 * @param other other pose
 * @return Pose
 */
Pose Pose::operator-(const Pose& other) { return Pose(x - other.x, y - other.y, theta); }

/**
 * @brief Set the value of this pose to its value - the value of another pose
 *
 * @param other other pose
 */
void Pose::operator-=(const Pose& other) {
    x -= other.x;
    y -= other.y;
    theta -= other.theta;
}

/**
 * @brief Multiply a pose by this pose
 *
 * @param other other pose
 * @return Pose
 */
float Pose::operator*(const Pose& other) { return (x * other.x + y * other.y).raw(); } // todo

/**
 * @brief Multiply a pose by a float
 *
 * @param other float
 * @return Pose
 */
Pose Pose::operator*(const float& other) { return Pose(x * other, y * other, theta); }

/**
 * @brief Divide a pose by a float
 *
 * @param other float
 * @return Pose
 */
Pose Pose::operator/(const float& other) { return Pose(x / other, y / other, theta); }

/**
 * @brief Check if two poses are equal
 *
 * @param other the other pose
 * @return bool
 */
bool Pose::operator==(const Pose& other) { return x == other.x && y == other.y && theta == other.theta; }

/**
 * @brief Check if two poses are not equal
 *
 * @param other the other pose
 * @return bool
 */
bool Pose::operator!=(const Pose& other) { return x != other.x || y != other.y || theta != other.theta; }

/**
 * @brief Linearly interpolate between two poses
 *
 * @param other the other pose
 * @param t t value
 * @return Pose
 */
Pose Pose::lerp(Pose other, float t) { return Pose(x + (other.x - x) * t, y + (other.y - y) * t, theta); }

/**
 * @brief Get the distance between two poses
 *
 * @param other the other pose
 * @return float
 */
Length Pose::distance(Pose other) { return units::sqrt(units::square(x - other.x) + units::square(y - other.y)); }

/**
 * @brief Get the angle between two poses
 *
 * @param other the other pose
 * @return float in radians
 */
Angle Pose::angle(Pose other) { return units::atan2(other.y - y, other.x - x); }

/**
 * @brief Rotate a pose by an angle
 *
 * @param angle angle in radians
 * @return Pose
 */
Pose Pose::rotate(Angle angle) {
    const Number cosAngle = units::cos(angle);
    const Number sinAngle = units::sin(angle);

    return Pose(x * cosAngle - y * sinAngle, x * sinAngle + y * cosAngle, theta);
}

std::string format_as(const Pose& pose) {
    // the double brackets become single brackets
    return fmt::format("lemlib::Pose {{ x: {}, y: {}, theta: {} }}", pose.x.convert(m), pose.y.convert(m),
                       pose.theta.convert(deg));
}
} // namespace lemlib