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
        Pose(Length x, Length y, Angle theta = 0_rad);
        /**
         * @brief Add a pose to this pose
         *
         * @param other other pose
         * @return Pose
         */
        Pose operator+(const Pose& other) const;
        /**
         * @brief Set the value of this pose to its value + the value of another pose
         *
         * @param other other pose
         */
        void operator+=(const Pose& other);
        /**
         * @brief Subtract a pose from this pose
         *
         * @param other other pose
         * @return Pose
         */
        Pose operator-(const Pose& other) const;
        /**
         * @brief Set the value of this pose to its value - the value of another pose
         *
         * @param other other pose
         */
        void operator-=(const Pose& other);
        /**
         * @brief Multiply a pose by this pose
         *
         * @param other other pose
         * @return Pose
         */
        float operator*(const Pose& other) const;
        /**
         * @brief Multiply a pose by a float
         *
         * @param other float
         * @return Pose
         */
        Pose operator*(const float& other) const;
        /**
         * @brief Divide a pose by a float
         *
         * @param other float
         * @return Pose
         */
        Pose operator/(const float& other) const;
        /**
         * @brief Check if two poses are equal
         *
         * @param other the other pose
         * @return bool
         */
        bool operator==(const Pose& other) const;
        /**
         * @brief Check if two poses are not equal
         *
         * @param other the other pose
         * @return bool
         */
        bool operator!=(const Pose& other) const;
        /**
         * @brief Linearly interpolate between two poses
         *
         * @param other the other pose
         * @param t t value
         * @return Pose
         */
        Pose lerp(const Pose other, float t) const;
        /**
         * @brief Get the distance between two poses
         *
         * @param other the other pose
         * @return Length
         */
        Length distance(Pose other) const;
        /**
         * @brief Get the angle between two poses
         *
         * @param other the other pose
         * @return Angle
         */
        Angle angle(Pose other) const;
        /**
         * @brief Rotate a pose by an angle
         *
         * @note positive angle results in counterclockwise translation
         *
         * @param angle angle
         * @return Pose
         */
        Pose rotate(Angle angle) const;
};

/**
 * @brief Format a pose
 *
 * @param pose
 * @return std::string
 */
std::string format_as(const Pose& pose);

/**
 * @brief Waypoint class. Derived from Pose. Has speed field
 */
class Waypoint : public Pose {
    public:
        /**
         * @brief Construct a new Waypoint
         *
         * @param x
         * @param y
         * @param theta
         * @param speed
         */
        Waypoint(Length x, Length y, Angle theta = 0_rad, double speed = 0, int index = 0)
            : Pose(x, y, theta),
              speed(speed),
              index(index) {}

        double speed;
        int index;
};
} // namespace lemlib
