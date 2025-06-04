#pragma once

#include "lemlib/config.hpp"
#include "lemlib/util.hpp"
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
struct TurnToParams {
        enum class LockedSide { LEFT, RIGHT };
        /** which side of the drivetrain to lock, if any. Used for swing turns */
        std::optional<LockedSide> lockedSide = std::nullopt;
        /** the direction the robot should turn in. AUTO by default */
        std::optional<AngularDirection> direction = std::nullopt;
        /** the maximum speed the robot can turn at. Value between 0-1. 1 by default */
        Number maxSpeed = 1;
        /** the minimum speed the robot can turn at. If set to a non-zero value, the `it conditions will switch to less
         * accurate but smoother ones. Value between 0-1. 0 by default */
        Number minSpeed = 0;
        /** how much the power (from -1 to +1) can increase per second. Defaults to 0 */
        Number slew = angular_slew;
        /** angle between the robot and target point where the movement will exit. Only has an effect if minSpeed is
         * non-zero.*/
        AngleRange earlyExitRange = 0_cRot;
};

/**
 * @brief Settings for Chassis::turnToHeading
 *
 * We use a struct to simplify customization. Chassis::turnToHeading has many
 * parts of the robot that need to be passed to it. By passing a struct to the
 * function, we can have named parameters, overcoming the c/c++ limitation
 */
struct TurnToSettings {
        /** the angular PID that is used to turn the robot */
        PID angularPID = angular_pid;
        /** the exit conditions that will cause the robot to stop moving */
        ExitConditionGroup<AngleRange> exitConditions = angular_exit_conditions;
        /** this function should return the estimated pose of the robot, typically by the tracking wheel odometry. */
        std::function<units::Pose()> poseGetter = pose_getter;
        /** the left motor group of the drivetrain */
        lemlib::MotorGroup& leftMotors = left_motors;
        /** the right motor group of the drivetrain */
        lemlib::MotorGroup& rightMotors = right_motors;
};

/**
 * @brief Turn the robot to face a heading or position
 *
 * @param target the target to turn to. Can be an angle, or a position
 * @param timeout the maximum amount of time the motion can run for
 * @param params struct containing parameters for the turn
 * @param settings struct containing settings for the turn
 */
void turnTo(std::variant<Angle, units::V2Position> target, Time timeout, TurnToParams params, TurnToSettings settings);
} // namespace lemlib