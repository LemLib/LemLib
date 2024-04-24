#pragma once

#include <string>

namespace lemlib {
/**
 * @brief A pose in 2D space
 *
 * A pose is a position and heading in 2D space.
 * Representing a pose as a single object makes it easier to work with,
 * especially in motion algorithms and position tracking.
 *
 * The Pose class overloads operators so you can easily add, subtract, multiply, etc.
 */
class Pose {
    public:
        float x;
        float y;
        float theta;
        /**
         * @brief Create a new pose
         *
         * @note Pose is unitless. It is up to the user to ensure that the units are consistent.
         *
         * @param x component
         * @param y component
         * @param theta heading. Defaults to 0
         *
         * @b Example
         * @code {.cpp}
         * // create a pose
         * lemlib::Pose poseA(5, // x position
         *                    10, // y position
         *                    1.57); // heading
         * // create a pose
         * lemlib::Pose poseB(5.2, 22); // x and y position, heading defaults to 0
         * @endcode
         */
        Pose(float x, float y, float theta = 0);
        /**
         * @brief Add a pose to this pose
         *
         * @note heading is not modified, and is taken from this pose
         *
         * @param other other pose
         * @return Pose
         *
         * @b Example
         * @code {.cpp}
         * // create a pose
         * lemlib::Pose poseA(5, 10, 1.57);
         * // create a pose
         * lemlib::Pose poseB(5.2, 22, 2.22);
         * // add the two poses
         * lemlib::Pose poseC = poseA + poseB;
         * // poseC.x = 10.2, poseC.y = 32, poseC.theta = 1.57
         * @endcode
         */
        Pose operator+(const Pose& other) const;
        /**
         * @brief Subtract a pose from this pose
         *
         * @note heading is not modified, and is taken from this pose
         *
         * @param other other pose
         * @return Pose
         *
         * @b Example
         * @code {.cpp}
         * // create a pose
         * lemlib::Pose poseA(5, 10, 1.57);
         * // create a pose
         * lemlib::Pose poseB(5.2, 22, 2.22);
         * // subtract the two poses
         * lemlib::Pose poseC = poseA - poseB;
         * // poseC.x = -0.2, poseC.y = -12, poseC.theta = 1.57
         * @endcode
         */
        Pose operator-(const Pose& other) const;
        /**
         * @brief Multiply a pose by this pose (dot product)
         *
         * @note heading is not considered in this operation
         *
         * @param other other pose
         * @return Pose
         *
         * @b Example
         * @code {.cpp}
         * // create a pose
         * lemlib::Pose poseA(5, 10, 1.57);
         * // create a pose
         * lemlib::Pose poseB(5.2, 22, 2.22);
         * // multiply the two poses
         * float result = poseA * poseB; // 246
         * @endcode
         */
        float operator*(const Pose& other) const;
        /**
         * @brief Multiply a pose by a float
         *
         * @note heading is not considered in this operation
         *
         * @param other float
         * @return Pose
         *
         * @b Example
         * @code {.cpp}
         * // create a pose
         * lemlib::Pose pose(1, 2);
         * // multiply the pose by 4.0
         * lemlib::Pose result = pose * 4.0;
         * // result.x = 4, result.y = 8
         * @endcode
         */
        Pose operator*(const float& other) const;
        /**
         * @brief Divide a pose by a float
         *
         * @note heading is not considered in this operation
         *
         * @param other float
         * @return Pose
         *
         * @b Example
         * @code {.cpp}
         * // create a pose
         * lemlib::Pose pose(6, 8);
         * // divide the pose by 2
         * lemlib::Pose result = pose / 2;
         * // result.x = 3, result.y = 4
         * @endcode
         */
        Pose operator/(const float& other) const;
        /**
         * @brief Linearly interpolate between two poses
         *
         * @note heading is not considered in this operation
         *
         * @param other the other pose
         * @param t t value
         * @return Pose
         *
         * @b Example
         * @code {.cpp}
         * // create poses
         * lemlib::Pose poseA(0, 0);
         * lemlib::Pose poseB(0, 2);
         * // find the pose in between the 2 poses
         * lemlib::Pose result = poseA.lerp(poseB, 0.5);
         * // result.x = 0, result.y = 1
         * @endcode
         */
        Pose lerp(Pose other, float t) const;
        /**
         * @brief Get the distance between two poses
         *
         * @note heading is not considered in this operation
         *
         * @param other the other pose
         * @return float
         *
         * @b Example
         * @code {.cpp}
         * // create poses
         * lemlib::Pose poseA(0, 0);
         * lemlib::Pose poseB(3, 4);
         * // find the distance between the poses
         * float result = poseA.distance(poseB); // result = 5
         * @endcode
         */
        float distance(Pose other) const;
        /**
         * @brief Get the angle between two poses
         *
         * @note heading is not considered in this operation
         *
         * @param other the other pose
         * @return float in radians
         *
         * @b Example
         * @code {.cpp}
         * // create poses
         * lemlib::Pose poseA(-1, -2);
         * lemlib::Pose poseB(2, 1);
         * // find angle between poses
         * float result = poseA.angle(poseB); // result = 0.785398
         * @endcode
         */
        float angle(Pose other) const;
        /**
         * @brief Rotate a pose by an angle
         *
         * @note heading is not considered in this operation
         *
         * @param angle angle in radians
         * @return Pose
         *
         * @b Example
         * @code {.cpp}
         * // create pose
         * lemlib::Pose pose(1, 0);
         * // rotate pose by pi/2
         * lemlib::Pose result = pose.rotate(M_PI_2);
         * // result.x = 0, result.y = 1
         * @endcode
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
