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
void Chassis::setPose(float x, float y, float theta, bool radians) {
    Pose pose(x, y, theta);
    this->setPose(pose, radians);
}

/**
 * Set the pose of the chassis
 *
 * This function is a wrapper for the Odometry::setPose() function
 * but it also transforms the pose to the format needed by the user
 */
void Chassis::setPose(Pose pose, bool radians) {
    if (!radians) pose.theta = degToRad(pose.theta);
    pose.theta = M_PI_2 - pose.theta;
    this->odom->setPose(pose);
}

/**
 * Get the pose of the chassis
 *
 * This function is a wrapper for the Odometry::getPose() function
 * but it also transforms the pose to the format needed by the user
 */
Pose Chassis::getPose(bool radians) const {
    Pose pose = this->odom->getPose();
    pose.theta = M_PI_2 - pose.theta;
    if (!radians) pose.theta = radToDeg(pose.theta);
    return pose;
}

/**
 * Wait until the robot has traveled a certain distance, or angle
 *
 * @note Units are in inches if current motion is moveTo or follow, degrees if using turnTo
 *
 * Just uses a while loop and exits when the distance traveled is greater than the specified distance
 * or if the motion has finished
 */
void Chassis::waitUntil(float dist) {
    // give the movement time to start
    pros::delay(10);
    // wait until the robot has travelled a certain distance
    while (this->movement != nullptr && this->movement->getDist() < dist && this->movement->getDist() >= prevDist) {
        this->prevDist = this->movement->getDist(); // update previous distance
        pros::delay(10);
    }
    // set prevDist to 0
    this->prevDist = 0;
}

/**
 * Wait until the robot has completed the current movement
 */
void Chassis::waitUntilDone() {
    // give the movement time to start
    pros::delay(10);
    // wait until the movement is done
    while (this->movement != nullptr && this->movement->getDist() >= prevDist) {
        this->prevDist = this->movement->getDist(); // update previous distance
        pros::delay(10);
    }
    // set prevDist to 0
    this->prevDist = 0;
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
