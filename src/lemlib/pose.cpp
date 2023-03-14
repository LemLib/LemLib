/**
 * @file lemlib/pose.cpp
 * @author Liam Teale
 * @brief Source file containing the implementation of the Pose class
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <math.h>
#include "lemlib/pose.hpp"


/**
 * @brief Create a new pose
 * 
 * @param x component
 * @param y component
 * @param theta heading. Defaults to 0
 */
lemlib::Pose::Pose(float x, float y, float theta) {
    this->x = x;
    this->y = y;
    this->theta = theta;
}


/**
 * @brief Add a pose to this pose
 * 
 * @param other other pose
 * @return Pose 
 */
lemlib::Pose lemlib::Pose::operator + (const lemlib::Pose &other) {
    return lemlib::Pose(this->x + other.x, this->y + other.y, this->theta);
}


/**
 * @brief Subtract a pose from this pose
 * 
 * @param other other pose
 * @return Pose 
 */
lemlib::Pose lemlib::Pose::operator - (const lemlib::Pose &other) {
    return lemlib::Pose(this->x - other.x, this->y - other.y, this->theta);
}


/**
 * @brief Multiply a pose by this pose
 * 
 * @param other other pose
 * @return Pose 
 */
float lemlib::Pose::operator * (const lemlib::Pose &other) {
    return this->x * other.x + this->y * other.y;
}


/**
 * @brief Multiply a pose by a float
 * 
 * @param other float
 * @return Pose 
 */
lemlib::Pose lemlib::Pose::operator * (const float &other) {
    return lemlib::Pose(this->x * other, this->y * other, this->theta);
}


/**
 * @brief Divide a pose by a float
 * 
 * @param other float
 * @return Pose 
 */
lemlib::Pose lemlib::Pose::operator / (const float &other) {
    return lemlib::Pose(this->x / other, this->y / other, this->theta);
}


/**
 * @brief Linearly interpolate between two poses
 * 
 * @param other the other pose
 * @param t t value
 * @return Pose 
 */
lemlib::Pose lemlib::Pose::lerp(lemlib::Pose other, float t) {
    return lemlib::Pose(this->x + (other.x - this->x) * t, this->y + (other.y - this->y) * t, this->theta);
}


/**
 * @brief Get the distance between two poses
 * 
 * @param other the other pose
 * @return float 
 */
float lemlib::Pose::distance(lemlib::Pose other) {
    return std::hypot(this->x - other.x, this->y - other.y);
}


/**
 * @brief Get the angle between two poses
 * 
 * @param other the other pose
 * @return float in radians
 */
float lemlib::Pose::angle(lemlib::Pose other) {
    return std::atan2(other.y - this->y, other.x - this->x);
}


/**
 * @brief Rotate a pose by an angle
 * 
 * @param angle angle in radians
 * @return Pose 
 */
lemlib::Pose lemlib::Pose::rotate(float angle) {
    return lemlib::Pose(this->x * std::cos(angle) - this->y * std::sin(angle), this->x * std::sin(angle) + this->y * std::cos(angle), this->theta);
}
