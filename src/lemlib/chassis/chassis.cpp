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
#include "pros/motors.hpp"
#include "pros/misc.hpp"
#include "lemlib/util.hpp"
#include "lemlib/pid.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/odom.hpp"
#include "trackingWheel.hpp"

/**
 * @brief Construct a new Chassis
 *
 * @param drivetrain drivetrain to be used for the chassis
 * @param lateralSettings settings for the lateral controller
 * @param angularSetting settings for the angular controller
 * @param sensors sensors to be used for odometry
 */
lemlib::Chassis::Chassis(Drivetrain_t drivetrain, ChassisController_t lateralSettings,
                         ChassisController_t angularSettings, OdomSensors_t sensors) {
    this->drivetrain = drivetrain;
    this->lateralSettings = lateralSettings;
    this->angularSettings = angularSettings;
    this->odomSensors = sensors;
}

/**
 * @brief Calibrate the chassis sensors
 *
 */
void lemlib::Chassis::calibrate() {
    // calibrate the imu if it exists
    if (odomSensors.imu != nullptr) {
        odomSensors.imu->reset(true);
        // keep on calibrating until it calibrates successfully
        while (errno == PROS_ERR || errno == ENODEV || errno == ENXIO) {
            pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "---");
            odomSensors.imu->reset(true);
            pros::delay(10);
        }
    }
    // initialize odom
    if (odomSensors.vertical1 == nullptr)
        odomSensors.vertical1 = new lemlib::TrackingWheel(drivetrain.leftMotors, drivetrain.wheelDiameter,
                                                          -(drivetrain.trackWidth / 2), drivetrain.rpm);
    if (odomSensors.vertical2 == nullptr)
        odomSensors.vertical2 = new lemlib::TrackingWheel(drivetrain.rightMotors, drivetrain.wheelDiameter,
                                                          drivetrain.trackWidth / 2, drivetrain.rpm);
    odomSensors.vertical1->reset();
    odomSensors.vertical2->reset();
    if (odomSensors.horizontal1 != nullptr) odomSensors.horizontal1->reset();
    if (odomSensors.horizontal2 != nullptr) odomSensors.horizontal2->reset();
    lemlib::setSensors(odomSensors, drivetrain);
    lemlib::init();
    // rumble to controller to indicate success
    pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, ".");
}

/**
 * @brief Set the Pose object
 *
 * @param x new x value
 * @param y new y value
 * @param theta new theta value
 * @param radians true if theta is in radians, false if not. False by default
 */
void lemlib::Chassis::setPose(double x, double y, double theta, bool radians) {
    lemlib::setPose(lemlib::Pose(x, y, theta), radians);
}

/**
 * @brief Set the pose of the chassis
 *
 * @param Pose the new pose
 * @param radians whether pose theta is in radians (true) or not (false). false by default
 */
void lemlib::Chassis::setPose(Pose pose, bool radians) { lemlib::setPose(pose, radians); }

/**
 * @brief Get the pose of the chassis
 *
 * @param radians whether theta should be in radians (true) or degrees (false). false by default
 * @return Pose
 */
lemlib::Pose lemlib::Chassis::getPose(bool radians) { return lemlib::getPose(radians); }

/**
 * @brief Turn the chassis so it is facing the target point
 *
 * The PID logging id is "angularPID"
 *
 * @param x x location
 * @param y y location
 * @param timeout longest time the robot can spend moving
 * @param reversed whether the robot should turn in the opposite direction. false by default
 * @param maxSpeed the maximum speed the robot can turn at. Default is 200
 */
void lemlib::Chassis::turnTo(float x, float y, int timeout, bool reversed, float maxSpeed) {
    Pose target(x, y);
    std::uint8_t compState = pros::competition::get_status();
    // create a new PID controller
    lemlib::FAPID pid = FAPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    pid.setExit(angularSettings.largeError, angularSettings.smallError, angularSettings.largeErrorTimeout,
                angularSettings.smallErrorTimeout, timeout);

    // loop until the PID settles or the competition mode changes
    while (pros::competition::get_status() == compState && !pid.settled()) {
        // calculate delta
        Pose pose = getPose();
        pose.theta = (reversed) ? fmod(pose.theta - M_PI, 2 * M_PI) : fmod(pose.theta, 2 * M_PI);
        target.theta = pose.angle(target);
        Pose delta = target - pose;

        // calculate speed
        float motorPower = pid.update(0, delta.theta);
        drivetrain.leftMotors->move(motorPower);
        drivetrain.rightMotors->move(-motorPower);

        pros::delay(10);
    }

    // stop the drivetrain
    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
}

/**
 * @brief Move the chassis to a target point
 *
 * The PID logging ids are "angularPID" and "lateralPID"
 *
 * @param x x location
 * @param y y location
 * @param timeout longest time the robot can spend moving
 * @param maxSpeed the maximum speed the robot can move at
 * @param reversed whether the robot should turn in the opposite direction. false by default
 * @param log whether the chassis should log the turnTo function. false by default
 */
void lemlib::Chassis::moveToPoint(float x, float y, int timeout, float maxSpeed, bool log) {
    Pose pose(0, 0);
    float prevLateralPower = 0;
    float prevAngularPower = 0;
    bool close = false;
    int start = pros::millis();
    std::uint8_t compState = pros::competition::get_status();

    // create a new PID controller
    FAPID lateralPID(0, 0, lateralSettings.kP, 0, lateralSettings.kD, "lateralPID");
    FAPID angularPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    lateralPID.setExit(lateralSettings.largeError, lateralSettings.smallError, lateralSettings.largeErrorTimeout,
                       lateralSettings.smallErrorTimeout, timeout);

    // main loop
    while (pros::competition::get_status() == compState && (!lateralPID.settled() || pros::millis() - start < 300)) {
        // get the current position
        Pose pose = getPose();
        pose.theta = std::fmod(pose.theta, 360);

        // update error
        float deltaX = x - pose.x;
        float deltaY = y - pose.y;
        float targetTheta = fmod(radToDeg(M_PI_2 - atan2(deltaY, deltaX)), 360);
        float hypot = std::hypot(deltaX, deltaY);
        float diffTheta1 = angleError(pose.theta, targetTheta);
        float diffTheta2 = angleError(pose.theta, targetTheta + 180);
        float angularError = (std::fabs(diffTheta1) < std::fabs(diffTheta2)) ? diffTheta1 : diffTheta2;
        float lateralError = hypot * cos(degToRad(std::fabs(diffTheta1)));

        // calculate speed
        float lateralPower = lateralPID.update(lateralError, 0, log);
        float angularPower = -angularPID.update(angularError, 0, log);

        // if the robot is close to the target
        if (pose.distance(lemlib::Pose(x, y)) < 7.5) {
            close = true;
            maxSpeed = (std::fabs(prevLateralPower) < 30) ? 30 : std::fabs(prevLateralPower);
        }

        // limit acceleration
        if (!close) lateralPower = lemlib::slew(lateralPower, prevLateralPower, lateralSettings.slew);
        if (std::fabs(angularError) > 25)
            angularPower = lemlib::slew(angularPower, prevAngularPower, angularSettings.slew);

        // cap the speed
        if (lateralPower > maxSpeed) lateralPower = maxSpeed;
        else if (lateralPower < -maxSpeed) lateralPower = -maxSpeed;
        if (close) angularPower = 0;

        prevLateralPower = lateralPower;
        prevAngularPower = angularPower;

        float leftPower = lateralPower + angularPower;
        float rightPower = lateralPower - angularPower;

        // ratio the speeds to respect the max speed
        float ratio = std::max(std::fabs(leftPower), std::fabs(rightPower)) / maxSpeed;
        if (ratio > 1) {
            leftPower /= ratio;
            rightPower /= ratio;
        }

        // move the motors
        drivetrain.leftMotors->move(leftPower);
        drivetrain.rightMotors->move(rightPower);

        pros::delay(10);
    }

    // stop the drivetrain
    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
}

/**
 * @brief Move the chassis to a target point
 *
 * The PID logging ids are "angularPID" and "lateralPID"
 *
 * @param x x location
 * @param y y location
 * @param heading the heading, in degrees
 * @param d the lead distance gain. 0 < d < 1. 0.6 recommended
 * @param timeout longest time the robot can spend moving
 * @param maxSpeed the maximum speed the robot can move at
 */
void lemlib::Chassis::moveToPose(float x, float y, float heading, float d, int timeout, float maxSpeed) {
    // initialize misc variables
    Pose target(x, y, M_PI_2 - degToRad(heading));
    std::uint8_t compState = pros::competition::get_status();
    bool close = false;
    // initialize PIDs
    FAPID linearPID(0, 0, lateralSettings.kP, 0, lateralSettings.kD, "linearPID");
    FAPID angularPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    linearPID.setExit(lateralSettings.largeError, lateralSettings.smallError, lateralSettings.largeErrorTimeout,
                      lateralSettings.smallErrorTimeout, timeout);

    // main loop
    while (pros::competition::get_status() == compState && !linearPID.settled()) {
        // get the current position
        Pose pose = getPose();
        pose.theta = M_PI_2 - pose.theta; // convert to standard form

        // calculate error
        Pose carrot = target - Pose(cos(target.theta), sin(target.theta)) * d * pose.distance(target);
        carrot.theta = pose.angle(carrot); // the theta of the carrot is the angle between the robot and the carrot
        // if the robot is close to the target, use the target as the carrot
        Pose delta = pose.distance(target) < lateralSettings.largeError ? target - pose : carrot - pose;
        float lateralError = delta.distance(Pose(0, 0)) * cos(delta.theta);

        // calculate speed
        float lateralPower = linearPID.update(lateralError, 0);
        float angularPower = angularPID.update(delta.theta, 0);
        lateralPower = std::clamp(lateralPower, -maxSpeed, maxSpeed); // cap the speed

        // calculate left and right motor power
        float leftPower = lateralPower + angularPower;
        float rightPower = lateralPower - angularPower;
        // balance the speeds to prevent motor saturation
        float ratio = std::max(std::fabs(leftPower), std::fabs(rightPower)) / maxSpeed;
        if (ratio > 1) {
            leftPower /= ratio;
            rightPower /= ratio;
        }

        // move the motors
        drivetrain.leftMotors->move(leftPower);
        drivetrain.rightMotors->move(rightPower);

        pros::delay(10);
    }
}
