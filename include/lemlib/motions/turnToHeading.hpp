#pragma once

#include "lemlib/ExitCondition.hpp"
#include "lemlib/util.hpp"
#include "pros/motor_group.hpp"
#include "units/Angle.hpp"
#include "units/units.hpp"
#include "units/Pose.hpp"
#include "lemlib/pid.hpp"
#include <functional>

namespace lemlib {
/**
 * @brief Parameters for Chassis::turnToHeading
 *
 * We use a struct to simplify customization. Chassis::turnToHeading has many
 * parameters and specifying them all just to set one optional param ruins
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 */
struct TurnToHeadingParams {
        /** the direction the robot should turn in. AUTO by default */
        AngularDirection direction = AngularDirection::AUTO;
        /** the maximum speed the robot can turn at. Value between 0-127. 127 by default */
        int maxSpeed = 127;
        /** the minimum speed the robot can turn at. If set to a non-zero value, the `it conditions will switch to less
         * accurate but smoother ones. Value between 0-127. 0 by default */
        int minSpeed = 0;
        /** angle between the robot and target point where the movement will exit. Only has an effect if minSpeed is
         * non-zero.*/
        Angle earlyExitRange = 0_cRot;
};

/**
 * @brief Settings for Chassis::turnToHeading
 *
 * We use a struct to simplify customization. Chassis::turnToHeading has many
 * parts of the robot that need to be passed to it. By passing a struct to the
 * function, we can have named parameters, overcoming the c/c++ limitation
 */
struct TurnToHeadingSettings {
        /** the angular PID that is used to turn the robot */
        PID angularPID;
        /** the exit conditions that will cause the robot to stop moving */
        ExitConditionGroup exitConditions;
        /** this function should return the estimated pose of the robot, typically by the tracking wheel odometry. */
        std::function<units::Pose()> poseGetter;
        /** the left motor group of the drivetrain */
        pros::MotorGroup& leftMotors;
        /** the right motor group of the drivetrain */
        pros::MotorGroup& rightMotors;
};

/**
 * @brief Turn the robot to face a specific heading
 *
 * @param heading the heading to turn to
 * @param timeout the longest time the robot can spend moving before exiting
 * @param params struct containing parameters for the turn
 * @param settings struct containing settings for the turn
 *
 * @b Example
 * @code {.cpp}
 * // turn to a heading of 135 degrees with a timeout of 1 second
 * turnToHeading(135_cDeg, 1_sec)
 * // turn to a heading of 230.5 degrees with a timeout of 2 seconds and a maximum speed of 60
 * turnToHeading(230.5_cDeg, 2_sec, { .maxSpeed = 60 })
 * // turn the robot to face -90 degrees with a timeout of 1.5 seconds
 * // and turn counterclockwise
 * chassis.turnToHeading(-90_cDeg, 1.5_sec, {.direction = AngularDirection::CCW_COUNTERCLOCKWISE});
 * // turn the robot to face 90 degrees with a timeout of 500ms
 * // with a minSpeed of 20 and a maxSpeed of 60
 * chassis.turnToHeading(90_cDeg, 500_msec, {.maxSpeed = 60, .minSpeed = 20});
 * // turn the robot to face 45 degrees with a timeout of 2 seconds
 * // and a minSpeed of 60, and exit the movement if the robot is within 5 degrees of the target
 * chassis.turnToHeading(45_cDeg, 2_sec, {.minSpeed = 60, .earlyExitRange = 5_cDeg});
 */
void turnToHeading(Angle heading, Time timeout, TurnToHeadingParams params, TurnToHeadingSettings settings);
} // namespace lemlib