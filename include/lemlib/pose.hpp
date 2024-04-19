#pragma once

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
        Pose operator+(const Pose& other) const;
        /**
         * @brief Subtract a pose from this pose
         *
         * @param other other pose
         * @return Pose
         */
        Pose operator-(const Pose& other) const;
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
         * @brief Linearly interpolate between two poses
         *
         * @param other the other pose
         * @param t t value
         * @return Pose
         */
        Pose lerp(Pose other, float t) const;
        /**
         * @brief Get the distance between two poses
         *
         * @param other the other pose
         * @return float
         */
        float distance(Pose other) const;
        /**
         * @brief Get the angle between two poses
         *
         * @param other the other pose
         * @return float in radians
         */
        float angle(Pose other) const;
        /**
         * @brief Rotate a pose by an angle
         *
         * @param angle angle in radians
         * @return Pose
         */
        Pose rotate(float angle) const;
};

/**
 * @brief Format a pose
 *
 * @param pose
 * @return std::string
 */
std::string format_as(const Pose& pose);
} // namespace lemlib
