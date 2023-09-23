#pragma once

#include <utility>
#include <vector>
#include "lemlib/pose.hpp"
#include "lemlib/asset.hpp"
#include "lemlib/chassis/structs.hpp"
#include "lemlib/movement/movement.hpp"

namespace lemlib {
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
        Waypoint(float x, float y, float theta, float speed)
            : Pose(x, y, theta),
              speed(speed) {}

        float speed;
};

/**
 * @brief Pure Pursuit class. Derived from Movement
 */
class PurePursuit : protected Movement {
    public:
        /**
         * @brief Construct a new Pure Pursuit movement
         *
         * @param drive drivetrain settings
         * @param path const reference to the asset containing the path
         * @param lookahead the lookahead distance. Units in inches. Recommended value is 15, but can be changed if
         * needed
         * @param timeout the maximum time the robot can spend moving
         * @param forwards whether the chassis should move forwards or backwards. True by default
         * @param maxSpeed the maximum speed the robot can move at. 127 by default
         */
        PurePursuit(Drivetrain_t drive, const asset& path, float lookahead, int timeout, bool forwards = true,
                    int maxSpeed = 127);

        /**
         * @brief Update the movement
         *
         * @note This function should be called in a loop
         * @note Units are in inches and radians. Angle 0 is right, and increases counterclockwise
         *
         * @param pose the position of the chassis
         *
         * @return std::pair<int, int> left and right motor power respectively. 128 means movement is done
         */
        std::pair<int, int> update(Pose pose) override;
    private:
        Drivetrain_t drive;
        std::vector<Waypoint> path;
        int startTime;
        float lookahead;
        int timeout;
        bool forwards;
        int maxSpeed;
};
}; // namespace lemlib