#include "units/Vector2D.hpp"

class Pose : public units::V2Position {
    private:
        Angle theta;
    public:
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
        Pose(Length x, Length y, Angle theta = 0 * deg);
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
};
