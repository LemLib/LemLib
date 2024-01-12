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
Pose::Pose(float x, float y, float theta)
    : x(x),
      y(y),
      theta(theta) {}

/**
 * @brief Add a pose to this pose
 *
 * @param other other pose
 * @return Pose
 */
Pose Pose::operator+(const Pose& other) const { return Pose(x + other.x, y + other.y, theta); }

/**
 * @brief Set the value of this pose to its value + the value of another pose
 *
 * @param other other pose
 */
void Pose::operator+=(const Pose& other) {
    this->x += other.x;
    this->y += other.y;
    this->theta += other.theta;
}

/**
 * @brief Subtract a pose from this pose
 *
 * @param other other pose
 * @return Pose
 */
Pose Pose::operator-(const Pose& other) const { return Pose(this->x - other.x, this->y - other.y, theta); }

/**
 * @brief Set the value of this pose to its value - the value of another pose
 *
 * @param other other pose
 */
void Pose::operator-=(const Pose& other) {
    this->x -= other.x;
    this->y -= other.y;
    this->theta -= other.theta;
}

/**
 * @brief Multiply a pose by this pose
 *
 * @param other other pose
 * @return Pose
 */
float Pose::operator*(const Pose& other) const { return this->x * other.x + this->y * other.y; }

/**
 * @brief Multiply a pose by a float
 *
 * @param other float
 * @return Pose
 */
Pose Pose::operator*(const float& other) const { return Pose(this->x * other, this->y * other, this->theta); }

/**
 * @brief Divide a pose by a float
 *
 * @param other float
 * @return Pose
 */
Pose Pose::operator/(const float& other) const { return Pose(this->x / other, this->y / other, this->theta); }

/**
 * @brief Check if two poses are equal
 *
 * @param other the other pose
 * @return bool
 */
bool Pose::operator==(const Pose& other) const {
    return this->x == other.x && this->y == other.y && this->theta == other.theta;
}

/**
 * @brief Check if two poses are not equal
 *
 * @param other the other pose
 * @return bool
 */
bool Pose::operator!=(const Pose& other) const {
    return this->x != other.x || this->y != other.y || this->theta != other.theta;
}

/**
 * @brief Linearly interpolate between two poses
 *
 * @param other the other pose
 * @param t t value
 * @return Pose
 */
Pose Pose::lerp(Pose other, float t) const {
    return Pose(this->x + (other.x - this->x) * t, this->y + (other.y - this->y) * t, this->theta);
}

/**
 * @brief Get the distance between two poses
 *
 * @param other the other pose
 * @return float
 */
float Pose::distance(Pose other) const { return std::hypot(this->x - other.x, this->y - other.y); }

/**
 * @brief Get the angle between two poses
 *
 * @param other the other pose
 * @return float in radians
 */
float Pose::angle(Pose other) const { return std::atan2(other.y - this->y, other.x - this->x); }

/**
 * @brief Rotate a pose by an angle
 *
 * @param angle angle in radians
 * @return Pose
 */
Pose Pose::rotate(float angle) const {
    const float cosAngle = std::cos(angle);
    const float sinAngle = std::sin(angle);

    return Pose(this->x * cosAngle - this->y * sinAngle, this->x * sinAngle + this->y * cosAngle, this->theta);
}

std::ostream& operator<<(std::ostream& os, const lemlib::Pose& pose) { return os << lemlib::format_as(pose).c_str(); }

std::string format_as(const lemlib::Pose& pose) {
    // the double brackets become single brackets
    return fmt::format("lemlib::Pose {{ x: {}, y: {}, theta: {} }}", pose.x, pose.y, pose.theta);
}
} // namespace lemlib