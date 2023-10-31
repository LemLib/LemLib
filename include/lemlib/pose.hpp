/**
 * @file include/lemlib/pose.hpp
 * @author LemLib Team
 * @brief Pose class declarations
 * @version 0.4.5
 * @date 2023-01-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "units.hpp"
#include <string>

namespace lemlib {
class Pose {
    public:
        /** @brief x value*/
        float x;
        /** @brief y value*/
        float y;
        /** @brief theta value*/
        float theta;
        /**
         * @brief Create a new pose
         *
         * @param x component
         * @param y component
         * @param theta heading. Defaults to 0
         */
        Pose(float x, float y, float theta = 0);
        /**
         * @brief Add a pose to this pose
         *
         * @param other other pose
         * @return Pose
         */
        Pose operator+(const Pose& other);
        /**
         * @brief Subtract a pose from this pose
         *
         * @param other other pose
         * @return Pose
         */
        Pose operator-(const Pose& other);
        /**
         * @brief Multiply a pose by this pose
         *
         * @param other other pose
         * @return Pose
         */
        float operator*(const Pose& other);
        /**
         * @brief Multiply a pose by a float
         *
         * @param other float
         * @return Pose
         */
        Pose operator*(const float& other);
        /**
         * @brief Divide a pose by a float
         *
         * @param other float
         * @return Pose
         */
        Pose operator/(const float& other);
        /**
         * @brief Linearly interpolate between two poses
         *
         * @param other the other pose
         * @param t t value
         * @return Pose
         */
        Pose lerp(Pose other, float t);
        /**
         * @brief Get the distance between two poses
         *
         * @param other the other pose
         * @return float
         */
        float distance(Pose other);
        /**
         * @brief Get the angle between two poses
         *
         * @param other the other pose
         * @return float in radians
         */
        float angle(Pose other);
        /**
         * @brief Rotate a pose by an angle
         *
         * @param angle angle in radians
         * @return Pose
         */
        Pose rotate(float angle);
};

class UnitPose {
    public:
        /** @brief x value*/
        Length x;
        /** @brief y value*/
        Length y;
        /** @brief theta value*/
        Angle theta;
        /**
         * @brief Create a new pose
         *
         * @param x component
         * @param y component
         * @param theta heading. Defaults to 0
         */
        UnitPose(Length x, Length y, Angle theta = 0_rad);
        /**
         * @brief Add a pose to this pose
         *
         * @param other other pose
         * @return Pose
         */
        UnitPose operator+(const UnitPose& other);
        /**
         * @brief Subtract a pose from this pose
         *
         * @param other other pose
         * @return Pose
         */
        UnitPose operator-(const UnitPose& other);
        /**
         * @brief Multiply a pose by this pose
         *
         * @param other other pose
         * @return Pose
         */
        Length operator*(const UnitPose& other);
        /**
         * @brief Multiply a pose by a float
         *
         * @param other float
         * @return Pose
         */
        UnitPose operator*(const float& other);
        /**
         * @brief Divide a pose by a float
         *
         * @param other float
         * @return Pose
         */
        UnitPose operator/(const float& other);
        /**
         * @brief Linearly interpolate between two poses
         *
         * @param other the other pose
         * @param t t value
         * @return Pose
         */
        UnitPose lerp(UnitPose other, float t);
        /**
         * @brief Get the distance between two poses
         *
         * @param other the other pose
         * @return float
         */
        Length distance(UnitPose other);
        /**
         * @brief Get the angle between two poses
         *
         * @param other the other pose
         * @return Length
         */
        Angle angle(UnitPose other);
        /**
         * @brief Rotate a pose by an angle
         *
         * @param angle angle in radians
         * @return Pose
         */
        UnitPose rotate(Angle angle);
};

// Isomorphism between Pose and UnitPose
inline Pose withoutUnits(const UnitPose& pose) {
    return lemlib::Pose(to_in(pose.x), to_in(pose.y), to_rad(pose.theta));
}

inline UnitPose withUnits(const Pose& pose) {
    return lemlib::UnitPose(from_in(pose.x), from_in(pose.y), from_rad(pose.theta));
}

/**
 * @brief Format a pose
 *
 * @param pose
 * @return std::string
 */
std::string format_as(const Pose& pose);
} // namespace lemlib
