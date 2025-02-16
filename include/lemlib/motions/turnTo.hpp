#pragma once

#include "units/Pose.hpp"
#include "hardware/Motor/MotorGroup.hpp"
#include "lemlib/ExitCondition.hpp"
#include "lemlib/util.hpp"
#include "lemlib/PID.hpp"
#include "units/Vector2D.hpp"
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
        /** the longest time the robot can spend moving before exiting. Optional */
        std::optional<Time> timeout = std::nullopt;
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
        Number slew = 0;
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
        PID angularPID;
        /** the exit conditions that will cause the robot to stop moving */
        ExitConditionGroup<AngleRange> exitConditions;
        /** this function should return the estimated pose of the robot, typically by the tracking wheel odometry. */
        std::function<units::Pose()> poseGetter;
        /** the left motor group of the drivetrain */
        lemlib::MotorGroup& leftMotors;
        /** the right motor group of the drivetrain */
        lemlib::MotorGroup& rightMotors;
};

/**
 * @brief Turn the robot to face a heading or position
 *
 * @param target the target to turn to. Can be an angle, or a position
 * @param params struct containing parameters for the turn
 * @param settings struct containing settings for the turn
 */
void turnTo(std::variant<Angle, units::V2Position> target, TurnToParams params, TurnToSettings settings);
} // namespace lemlib