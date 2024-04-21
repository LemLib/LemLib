#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include "lemlib/pose.hpp"

lemlib::Pose::Pose(float x, float y, float theta) {
    this->x = x;
    this->y = y;
    this->theta = theta;
}

lemlib::Pose lemlib::Pose::operator+(const lemlib::Pose& other) const {
    return lemlib::Pose(this->x + other.x, this->y + other.y, this->theta);
}

lemlib::Pose lemlib::Pose::operator-(const lemlib::Pose& other) const {
    return lemlib::Pose(this->x - other.x, this->y - other.y, this->theta);
}

float lemlib::Pose::operator*(const lemlib::Pose& other) const { return this->x * other.x + this->y * other.y; }

lemlib::Pose lemlib::Pose::operator*(const float& other) const {
    return lemlib::Pose(this->x * other, this->y * other, this->theta);
}

lemlib::Pose lemlib::Pose::operator/(const float& other) const {
    return lemlib::Pose(this->x / other, this->y / other, this->theta);
}

lemlib::Pose lemlib::Pose::lerp(lemlib::Pose other, float t) const {
    return lemlib::Pose(this->x + (other.x - this->x) * t, this->y + (other.y - this->y) * t, this->theta);
}

float lemlib::Pose::distance(lemlib::Pose other) const { return std::hypot(this->x - other.x, this->y - other.y); }

float lemlib::Pose::angle(lemlib::Pose other) const { return std::atan2(other.y - this->y, other.x - this->x); }

lemlib::Pose lemlib::Pose::rotate(float angle) const {
    return lemlib::Pose(this->x * std::cos(angle) - this->y * std::sin(angle),
                        this->x * std::sin(angle) + this->y * std::cos(angle), this->theta);
}

std::string lemlib::format_as(const lemlib::Pose& pose) {
    // the double brackets become single brackets
    return fmt::format("lemlib::Pose {{ x: {}, y: {}, theta: {} }}", pose.x, pose.y, pose.theta);
}
