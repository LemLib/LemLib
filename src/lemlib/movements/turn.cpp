#include <cmath>
#include <bits/stdc++.h>
#include "pros/misc.hpp"
#include "pros/rtos.hpp"
#include "lemlib/util.hpp"
#include "lemlib/pose.hpp"
#include "lemlib/movements/turn.hpp"

namespace lemlib {
/**
 * Turn constructor
 *
 * Some members of the class need to be explicitly initialized
 * But, some members need to be configured further in the body
 *
 * Here we just store the arguments in member variables, and store the
 * initial competition state.
 */
Turn::Turn(FAPID<Angle> angularPID, Angle target, int maxSpeed)
    : angularPID(angularPID),
      targetHeading(target),
      maxSpeed(maxSpeed) {
    // get the current competition state. If this changes, the movement will stop
    compState = pros::competition::get_status();
}

/**
 * Turn constructor
 *
 * Some members of the class need to be explicitly initialized
 * But, some members need to be configured further in the body
 *
 * Here we just store the arguments in member variables, and store the
 * initial competition state.
 */
Turn::Turn(FAPID<Angle> angularPID, Pose target, bool reversed, int maxSpeed)
    : angularPID(angularPID),
      targetPose(target),
      reversed(reversed),
      maxSpeed(maxSpeed) {
    // get the current competition state. If this changes, the movement will stop
    compState = pros::competition::get_status();
}

/**
 * Get the distance travelled during the movement
 *
 * This is useful if you want to wait until the robot has travelled a certain distance.
 * For example, you want the robot to engage a mechanism when it has travelled 10 inches.
 */
float Turn::getDist() { return dist.convert(deg); }

/**
 * The turning algorithm uses field-relative position of the robot to face a target heading
 * or face a target point.
 *
 * This algorithm is simple. When the robot needs to face a target heading, it simply aligns
 * the robot's heading with the target heading. When the robot is turning to face a point,
 * the algorithm will align the robot's heading with the target point. This is better for
 * repeatability, but is not always necessary.
 *
 * This algorithm only uses 1 PID to turn the chassis.
 */
std::pair<int, int> Turn::update(Pose pose) {
    // set state to 1 if the pid has settled
    if (angularPID.settled()) state = 1;
    // exit if movement is in state 1 (settled)
    if (state == 1) return {128, 128};

    // reverse heading if doing movement in reverse
    if (reversed) pose.theta = units::mod(pose.theta - rot / 2, rot);

    // update completion vars
    if (dist == 0_deg) { // if dist is 0, this is the first time update() has been called
        dist = 0.0001_deg;
        startPose = pose;
    }
    dist = angleError(pose.theta, startPose.theta);

    if (targetPose != std::nullopt) targetHeading = pose.angle(targetPose.value());

    // calculate error
    Angle error = angleError(targetHeading, pose.theta);

    // calculate the speed
    float output = angularPID.update(0_deg, error); // todo: test
    // cap the speed
    output = std::clamp(int(std::round(output)), -maxSpeed, maxSpeed);
    // return output
    return {output, -output};
}
}; // namespace lemlib