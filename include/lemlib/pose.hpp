#pragma once

#include <string>
#include <ostream>

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
         * @return float
         */
<<<<<<< HEAD
        float distance(Pose other) const;
=======
        float distance(const Pose other) const;
>>>>>>> dfbd688 (Make appropriate pose methods const)
        /**
         * @brief Get the angle between two poses
         *
         * @param other the other pose
         * @return float in radians
         */
<<<<<<< HEAD
        float angle(Pose other) const;
=======
        float angle(const Pose other) const;
>>>>>>> dfbd688 (Make appropriate pose methods const)
        /**
         * @brief Rotate a pose by an angle
         *
         * @note positive angle results in counterclockwise translation
         *
         * @param angle angle in radians
         * @return Pose
         */
        Pose rotate(float angle) const;
};

/**
 * @brief output and format pose into a stream
 *
 * @param pose
 * @return std::ostream
 */
std::ostream& operator<<(std::ostream& os, const lemlib::Pose& pose);

/**
 * @brief Format a pose
 *
 * @param pose
 * @return std::string
 */
std::string format_as(const Pose& pose);

/**
 * @brief Format a pose
 *
 * @param pose
 * @return std::string
 */
<<<<<<< HEAD
std::string format_as(const Pose& pose);
} // namespace lemlib
=======
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
        Waypoint(float x, float y, float theta = 0, float speed = 0, int index = 0)
            : Pose(x, y, theta),
              speed(speed),
              index(index) {}

        float speed;
        int index;
};
} // namespace lemlib
>>>>>>> 05b4576 (Fixing Merge Conflicts)
