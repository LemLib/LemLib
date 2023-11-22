/**
 * @file src/lemlib/chassis/chassis.cpp
 * @author LemLib Team
 * @brief definitions for the chassis class
 * @version 0.4.5
 * @date 2023-01-27
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <math.h>
#include "lemlib/util.hpp"
#include "lemlib/chassis/chassis.hpp"

namespace lemlib {
/**
 * Set the pose of the chassis
 *
 * This function is a wrapper for the Odometry::setPose() function
 */
void Chassis::setPose(Length x, Length y, Angle theta) { odom->setPose({x, y, theta}); }

/**
 * Set the pose of the chassis
 *
 * This function is a wrapper for the Odometry::setPose() function
 * but it also transforms the pose to the format needed by the user
 */
void Chassis::setPose(Pose pose) { odom->setPose(pose); }

/**
 * Get the pose of the chassis
 *
 * This function is a wrapper for the Odometry::getPose() function
 * but it also transforms the pose to the format needed by the user
 */
Pose Chassis::getPose() { return odom->getPose(); }

/**
 * Wait until the robot has completed the current movement
 */
void Chassis::waitUntilDone() {
    // give the movement time to start
    pros::delay(10);
    // wait until the movement is done
    while (movement != nullptr && movement->getDist() >= prevDist) {
        prevDist = movement->getDist(); // update previous distance
        pros::delay(10);
    }
    // set prevDist to 0
    prevDist = 0;
}

/**
 * Move the robot with a custom motion algorithm
 */
void Chassis::moveCustom(std::unique_ptr<Movement> movement) {
    // if a movement is already running, wait until it is done
    if (movement != nullptr) waitUntilDone();
    // create the movement
    this->movement = std::move(movement);
}
}; // namespace lemlib
