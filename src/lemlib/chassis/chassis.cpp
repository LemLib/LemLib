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
#include "main.h"
#include "pros/motors.hpp"
#include "pros/misc.hpp"
#include "lemlib/util.hpp"
#include "lemlib/pid.hpp"
#include "lemlib/movements/boomerang.hpp"
#include "lemlib/movements/purepursuit.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/odom.hpp"
#include "lemlib/chassis/trackingWheel.hpp"

using namespace lemlib;

/**
 * Initialize the chassis
 *
 * Calibrates sensors and starts the chassis task
 */
void Chassis::initialize() {
    // calibrate odom
    odom.calibrate();
    // start the chassis task if it doesn't exist
    if (task == nullptr)
        task = new pros::Task([&]() {
            while (true) {
                update();
                pros::delay(10);
            }
        });
}

/**
 * Set the pose of the chassis
 *
 * This function is a wrapper for the Odometry::setPose() function
 */
void Chassis::setPose(float x, float y, float theta, bool radians) {
    Pose pose(x, y, theta);
    setPose(pose, radians);
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
    odom.setPose(pose);
}

/**
 * Get the pose of the chassis
 *
 * This function is a wrapper for the Odometry::getPose() function
 * but it also transforms the pose to the format needed by the user
 */
Pose Chassis::getPose(bool radians) {
    Pose pose = odom.getPose();
    pose.theta = M_PI_2 - pose.theta;
    if (!radians) pose.theta = radToDeg(pose.theta);
    return pose;
}

/**
 * Wait until the robot has traveled a certain distance during a movement
 *
 * @note Units are in inches if current motion is moveTo or follow, degrees if using turnTo
 *
 * Just uses a while loop and exits when the distance traveled is greater than the specified distance
 * or if the motion has finished
 */
void Chassis::waitUntilDist(float dist) {
    // give the movement time to start
    pros::delay(10);
    // wait until the movement is done
    while (movement != nullptr && movement->getDist() < dist && movement->getDist() >= prevDist) {
        prevDist = movement->getDist(); // update previous distance
        pros::delay(10);
    }
    // set prevDist to 0
    prevDist = 0;
}

/**
 * @brief Turn the chassis so it is facing the target point
 *
 * The PID logging id is "angularPID"
 *
 * @param x x location
 * @param y y location
 * @param timeout longest time the robot can spend moving
 * @param async whether the function should be run asynchronously. false by default
 * @param reversed whether the robot should turn to face the point with the back of the robot. false by default
 * @param maxSpeed the maximum speed the robot can turn at. Default is 200
 * @param log whether the chassis should log the turnTo function. false by default
 */
void Chassis::turnTo(float x, float y, int timeout, bool async, bool reversed, float maxSpeed, bool log) {
    // try to take the mutex
    // if its unsuccessful after 10ms, return
    if (!mutex.take(10)) return;
    // if the function is async, run it in a new task
    if (async) {
        pros::Task task([&]() { turnTo(x, y, timeout, false, reversed, maxSpeed, log); });
        mutex.give();
        pros::delay(10); // delay to give the task time to start
        return;
    }
    float targetTheta;
    float deltaX, deltaY, deltaTheta;
    float motorPower;
    float startTheta = getPose().theta;
    std::uint8_t compState = pros::competition::get_status();
    distTravelled = 0;

    // create a new PID controller
    FAPID pid = FAPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    pid.setExit(angularSettings.largeError, angularSettings.smallError, angularSettings.largeErrorTimeout,
                angularSettings.smallErrorTimeout, timeout);

    // main loop
    while (pros::competition::get_status() == compState && !pid.settled()) {
        // update variables
        Pose pose = getPose();
        pose.theta = (reversed) ? fmod(pose.theta - 180, 360) : fmod(pose.theta, 360);

        // update completion vars
        distTravelled = fabs(angleError(pose.theta, startTheta));

        deltaX = x - pose.x;
        deltaY = y - pose.y;
        targetTheta = fmod(radToDeg(M_PI_2 - atan2(deltaY, deltaX)), 360);

        // calculate deltaTheta
        deltaTheta = angleError(targetTheta, pose.theta);

        // calculate the speed
        motorPower = pid.update(0, deltaTheta, log);

        // cap the speed
        if (motorPower > maxSpeed) motorPower = maxSpeed;
        else if (motorPower < -maxSpeed) motorPower = -maxSpeed;

        // move the drivetrain
        drivetrain.leftMotors->move(-motorPower);
        drivetrain.rightMotors->move(motorPower);

        pros::delay(10);
    }

    // stop the drivetrain
    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
    // set distTraveled to -1 to indicate that the function has finished
    distTravelled = -1;
    // give the mutex back
    mutex.give();
}

/**
 * This function sets up the Boomerang controller
 *
 * Like all chassis movement functions, it sets a member pointer to a new movement.
 * the movement is a derived class of the Movement class.
 *
 * There are some things that need to be done before instantiating the movement however.
 * Two PIDs need to be set up to be passed to the Boomerang constructor, and the target heading
 * needs to be converted to radians and standard form.
 * It also needs to decide what the chasePower should be. Usually this will be the value set in
 * the drivetrain struct, but it can be overridden by the user if needed.
 */
void Chassis::moveTo(float x, float y, float theta, int timeout, bool forwards, float chasePower, float lead,
                     int maxSpeed) {
    // if a movement is already running, return
    if (movement == nullptr) return;
    // convert target theta to radians and standard form
    Pose target = Pose(x, y, M_PI_2 - degToRad(theta));
    // set up PIDs
    FAPID linearPID(0, 0, lateralSettings.kP, 0, lateralSettings.kD, "linearPID");
    linearPID.setExit(lateralSettings.largeError, lateralSettings.smallError, lateralSettings.largeErrorTimeout,
                      lateralSettings.smallErrorTimeout, timeout);
    FAPID angularPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    // if chasePower is 0, is the value defined in the drivetrain struct
    if (chasePower == 0) chasePower = drivetrain.chasePower;
    // create the movement
    movement = new Boomerang(linearPID, angularPID, target, timeout, forwards, chasePower, lead, maxSpeed);
}

/**
 * This function sets up Pure Pursuit
 *
 * Unlike the chassis::moveTo function, we can just pass the parameters directly to the
 * Pure Pursuit constructor
 */
void Chassis::follow(const asset& path, float lookahead, int timeout, bool forwards, int maxSpeed) {
    // if a movement is already running, return
    if (movement == nullptr) return;
    // create the movement
    movement = new PurePursuit(drivetrain.trackWidth, path, lookahead, timeout, forwards, maxSpeed);
}

/**
 * Chassis update function
 *
 * This function is called in a loop by the chassis task
 * It updates any motion controller that may be running
 * And it updates the odometry
 * Once implemented, it will also update the drivetrain velocity controllers
 */
void Chassis::update() {
    // update odometry
    odom.update();
    // update the motion controller, if one is running
    if (movement != nullptr) {
        std::pair<int, int> output = movement->update(odom.getPose()); // get output
        if (output.first == 128 && output.second == 128) { // if the movement is done
            movement = nullptr; // stop movement
            output.first = 0;
            output.second = 0;
        }
        // move the motors
        drivetrain.leftMotors->move(output.first);
        drivetrain.rightMotors->move(output.second);
    }
}
