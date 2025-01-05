#pragma once

#include "lemlib/ExitCondition.hpp"
#include "lemlib/util.hpp"
#include "units/Angle.hpp"
#include "units/Vector2D.hpp"
#include "units/units.hpp"
#include "units/Pose.hpp"
#include "lemlib/PID.hpp"
#include "hardware/Motor/MotorGroup.hpp"
#include <functional>

namespace lemlib {
/**
 * @brief Enum class DriveSide
 *
 * When using swing turns, the user needs to specify what side of the drivetrain should be locked
 * we could just use an integer or boolean for this, but using an enum class improves readability
 *
 * This enum class only has 2 values, LEFT and RIGHT
 */
enum class DriveSide {
    LEFT, /** lock the left side of the drivetrain */
    RIGHT /** lock the right side of the drivetrain */
};

/**
 * @brief Parameters for turnToAny
 *
 * We use a struct to simplify customization. Chassis::turnToHeading has many
 * parameters and specifying them all just to set one optional param ruins
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 */
struct TurnToAnyParams {
        /** @todo Document */
        std::optional<lemlib::DriveSide> lockedSide;
        /** the direction the robot should turn in. AUTO by default */
        AngularDirection direction = AngularDirection::AUTO;
        /** the maximum speed the robot can turn at. Value between 0-1. 1 by default */
        double maxSpeed = 1;
        /** the minimum speed the robot can turn at. If set to a non-zero value, the `it conditions will switch to less
         * accurate but smoother ones. Value between 0-1. 0 by default */
        double minSpeed = 0;
        /** angle between the robot and target point where the movement will exit. Only has an effect if minSpeed is
         * non-zero.*/
        AngleRange earlyExitRange = 0_cRot;
};

/**
 * @brief Settings for turnToAny
 *
 * We use a struct to simplify customization. Chassis::turnToHeading has many
 * parts of the robot that need to be passed to it. By passing a struct to the
 * function, we can have named parameters, overcoming the c/c++ limitation
 */
struct TurnToAnySettings {
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
 * @brief Parameters for Chassis::moveToPoint
 *
 * We use a struct to simplify customization. Chassis::moveToPoint has many
 * parameters and specifying them all just to set one optional param harms
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 */
struct TurnToPointParams : TurnToAnyParams {
        /** whether the robot should move forwards or backwards. True by default */
        bool forwards = true;
};

/**
 * @warning This is not intended to be used by ordinary users. It is instead to be used to share functionality between
 * the various turning motions.
 *
 * @brief Turn the robot to face an arbitrary heading with an arbitrary method.
 *
 * @param targetSource Produces the target heading for the turn to algorithm
 * @param output Consumes the output speed from the turn to algorithm (0 to 1)
 * @param timeout the longest time the robot can spend moving before exiting
 * @param params struct containing parameters for the turn
 * @param settings struct containing settings for the turn
 *
 * @todo investigate using OOP and inheritance as a cleaner method
 */
void turnToAny(std::function<Angle()> targetSource, Time timeout, TurnToAnyParams& params, TurnToAnySettings& settings);

/**
 * @brief Turn the chassis so it is facing the target heading
 *
 * @param theta heading location
 * @param timeout longest time the robot can spend moving
 * @param params struct to simulate named parameters
 * @param async whether the function should be run asynchronously. true by default
 *
 * @todo Update example
 * @b Example
 * @code {.cpp}
 * chassis.setPose(0, 0, 0); // set the pose of the chassis to x = 0, y = 0, theta = 0
 * // turn the robot to face heading 135, with a timeout of 1000ms
 * chassis.turnToHeading(135, 1000);
 * // turn the robot to face heading 230.5 with a timeout of 2000ms
 * // and a maximum speed of 60
 * chassis.turnToHeading(230.5, 2000, {.maxSpeed = 60});
 * // turn the robot to face heading -90 with a timeout of 1500ms
 * // and turn counterclockwise
 * chassis.turnToHeading(-90, 1500, {.direction = AngularDirection::CCW_COUNTERCLOCKWISE});
 * // turn the robot to face heading 90 with a timeout of 500ms
 * // with a minSpeed of 20 and a maxSpeed of 60
 * chassis.turnToHeading(90, 500, {.maxSpeed = 60, .minSpeed = 20});
 * // turn the robot to face heading 45 with a timeout of 2000ms
 * // and a minSpeed of 60, and exit the movement if the robot is within 5 degrees of the target
 * chassis.turnToHeading(45, 2000, {.minSpeed = 60, .earlyExitRange = 5});
 * @endcode
 */
void turnTo(Angle heading, Time timeout, TurnToAnyParams params, TurnToAnySettings settings);

/**
 * @brief Turn the chassis so it is facing the target point
 *
 * @param x x location
 * @param y y location
 * @param timeout longest time the robot can spend moving
 * @param params struct to simulate named parameters
 * @param async whether the function should be run asynchronously. true by default
 *
 * @todo Update example
 * @b Example
 * @code {.cpp}
 * chassis.setPose(0, 0, 0); // set the pose of the chassis to x = 0, y = 0, theta = 0
 * // turn the robot to face the point x = 45, y = -45, with a timeout of 1000ms
 * chassis.turnToPoint(45, -45, 1000);
 * // turn the robot to face the point x = 45, y = -45, with a timeout of 1000ms
 * // but face the point with the back of the robot
 * chassis.turnToPoint(45, -45, 1000, {.forwards = false});
 * // turn the robot to face the point x = -20, 32.5 with a timeout of 2000ms
 * // and a maximum speed of 60
 * chassis.turnToPoint(-20, 32.5, 2000, {.maxSpeed = 60});
 * // turn the robot to face the point x = -30, y = 22.5 with a timeout of 1500ms
 * // and turn counterclockwise
 * chassis.turnToPoint(-30, 22.5, 1500, {.direction = AngularDirection::CCW_COUNTERCLOCKWISE});
 * // turn the robot to face the point x = 10, y = 10 with a timeout of 500ms
 * // with a minSpeed of 20 and a maxSpeed of 60
 * chassis.turnToPoint(10, 10, 500, {.maxSpeed = 60, .minSpeed = 20});
 * // turn the robot to face the point x = 7.5, y = 7.5 with a timeout of 2000ms
 * // and a minSpeed of 60, and exit the movement if the robot is within 5 degrees of the target
 * chassis.turnToPoint(7.5, 7.5, 2000, {.minSpeed = 60, .earlyExitRange = 5});
 * @endcode
 */
void turnTo(units::Vector2D<Length> target, Time timeout, TurnToPointParams params, TurnToAnySettings settings);
} // namespace lemlib