/**
 * @file lemlib/chassis/chassis.cpp
 * @author Liam Teale
 * @brief definitions for the chassis class
 * @version 0.1
 * @date 2023-01-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <cstdint>
#include <math.h>
#include "lemlib/logger.hpp"
#include "pros/adi.h"
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
lemlib::Chassis::Chassis(Drivetrain_t drivetrain, ChassisController_t lateralSettings, ChassisController_t angularSettings, OdomSensors_t sensors)
{
    /* Drivetrain checks */
    for (int i = 0; i < drivetrain.leftMotors->size(); i++) {
        pros::Motor leftMotor = drivetrain.leftMotors->at(i);
        for (int j = 0; j < drivetrain.rightMotors->size(); j++) {
            pros::Motor rightMotor = drivetrain.rightMotors->at(j);
            if (leftMotor.get_port() == rightMotor.get_port()) lemlib::logger::error("Left and right motors have the same port: " + std::to_string(leftMotor.get_port()));
        }
    }

    if (drivetrain.trackWidth < 5) lemlib::logger::warn("Track width is too small: " + std::to_string(drivetrain.trackWidth));
    else if (drivetrain.trackWidth > 24) lemlib::logger::warn("Track width is too large: " + std::to_string(drivetrain.trackWidth));

    if (drivetrain.wheelDiameter < 2.5 || drivetrain.wheelDiameter > 4.5) lemlib::logger::warn("Wheel diameter is not valid: " + std::to_string(drivetrain.wheelDiameter) + " [Valid: 2.5-4.5\"]");
    
    if (drivetrain.rpm < 100) lemlib::logger::warn("Drivetrain RPM too small: " + std::to_string(drivetrain.rpm));

    /* Odometry checks */
    if (
        sensors.horizontal1 == nullptr && sensors.horizontal2 == nullptr &&
        sensors.vertical1 == nullptr && sensors.vertical2 == nullptr &&
        sensors.imu == nullptr
    ) lemlib::logger::warn("No sensors were provided for odometry");

    this->drivetrain = drivetrain;
    this->lateralSettings = lateralSettings;
    this->angularSettings = angularSettings;
    this->odomSensors = sensors;
}


/**
 * @brief Calibrate the chassis sensors
 * 
 */
void lemlib::Chassis::calibrate()
{
    // calibrate the imu if it exists
    std::int32_t imuStatus = 0;
    if (odomSensors.imu != nullptr) imuStatus = odomSensors.imu->reset(true);
    if (imuStatus == PROS_ERR) {
        lemlib::logger::error("Failed to reset IMU [Errno " + std::to_string(imuStatus) + "]");
        this->calibrated = false;
        return;
    }
    // initialize odom
    if (odomSensors.vertical1 == nullptr) odomSensors.vertical1 = new lemlib::TrackingWheel(drivetrain.leftMotors, drivetrain.wheelDiameter, -(drivetrain.trackWidth/2), drivetrain.rpm);
    if (odomSensors.vertical2 == nullptr) odomSensors.vertical2 = new lemlib::TrackingWheel(drivetrain.rightMotors, drivetrain.wheelDiameter, drivetrain.trackWidth/2, drivetrain.rpm);
    odomSensors.vertical1->reset();
    odomSensors.vertical2->reset();
    if (odomSensors.horizontal1 != nullptr) odomSensors.horizontal1->reset();
    if (odomSensors.horizontal2 != nullptr) odomSensors.horizontal2->reset();
    lemlib::setSensors(odomSensors, drivetrain);

    this->calibrated = true;

    lemlib::init();
}

/**
 * @brief Check if the chassis is calibrated
 * 
 * @return true if the chassis is calibrated
 */
bool lemlib::Chassis::isCalibrated()
{
    return this->calibrated;
}

/**
 * @brief Set the Pose object
 * 
 * @param x new x value
 * @param y new y value
 * @param theta new theta value
 * @param radians true if theta is in radians, false if not. False by default
 */
void lemlib::Chassis::setPose(double x, double y, double theta, bool radians)
{
    lemlib::logger::debug("Location set to: (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(theta) + ")");

    lemlib::setPose(lemlib::Pose(x, y, theta), radians);
}


/**
 * @brief Set the pose of the chassis 
 *
 * @param Pose the new pose
 * @param radians whether pose theta is in radians (true) or not (false). false by default
 */
void lemlib::Chassis::setPose(Pose pose, bool radians)
{
    lemlib::setPose(pose, radians);
}


/**
 * @brief Get the pose of the chassis
 * 
 * @param radians whether theta should be in radians (true) or degrees (false). false by default
 * @return Pose 
 */
lemlib::Pose lemlib::Chassis::getPose(bool radians)
{
    return lemlib::getPose(radians);
}


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
 * @param log whether the chassis should log the turnTo function. false by default
 */
void lemlib::Chassis::turnTo(float x, float y, int timeout, bool reversed, float maxSpeed, bool log)
{
    if (!this->isCalibrated()) {
        lemlib::logger::error("Chassis is attempting to turn before calibration - Check other logs, the chassis may have failed to calibrate");
        return;
    }

    if (maxSpeed < 20) lemlib::logger::warn("Max speed is too small: " + std::to_string(maxSpeed));

    if (lemlib::pointInBound(x, y, -100, 100)) lemlib::logger::warn("Target point moves out of the field: " + std::to_string(x) + ", " + std::to_string(y) + " [Valid: -100-100]");

    Pose pose = getPose();

    if (pose.distance(lemlib::Pose(x, y)) < 1) lemlib::logger::warn("Target point is too close to the robot: " + std::to_string(x) + ", " + std::to_string(y));

    float targetTheta;
    float deltaX, deltaY, deltaTheta;
    float motorPower;
    std::uint8_t compState = pros::competition::get_status();

    // create a new PID controller
    FAPID pid = FAPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    pid.setExit(angularSettings.largeError, angularSettings.smallError, angularSettings.largeErrorTimeout, angularSettings.smallErrorTimeout, timeout);

    // main loop
    while (pros::competition::get_status() == compState && !pid.settled()) {
        // update variables
        pose = getPose();
        pose.theta = (reversed) ? fmod(pose.theta-180, 360) : fmod(pose.theta, 360);
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
}


/**
 * @brief Move the chassis towards the target point
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
void lemlib::Chassis::moveTo(float x, float y, int timeout, float maxSpeed, bool log)
{
    if (!this->isCalibrated()) {
        lemlib::logger::error("Chassis is attempting to move before calibration - Check other logs, the chassis may have failed to calibrate");
        return;
    }

    if (maxSpeed < 20) lemlib::logger::warn("Max speed is too small: " + std::to_string(maxSpeed));

    if (lemlib::pointInBound(x, y, -100, 100)) lemlib::logger::warn("Target point moves out of the field: " + std::to_string(x) + ", " + std::to_string(y) + " [Valid: -100-100]");

    Pose pose = getPose();

    if (pose.distance(lemlib::Pose(x, y)) < 1) lemlib::logger::warn("Target point is too close to the robot: " + std::to_string(x) + ", " + std::to_string(y));

    float prevLateralPower = 0;
    float prevAngularPower = 0;
    bool close = false;
    int start = pros::millis();
    std::uint8_t compState = pros::competition::get_status();

    // create a new PID controller
    FAPID lateralPID(0, 0, lateralSettings.kP, 0, lateralSettings.kD, "lateralPID");
    FAPID angularPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    lateralPID.setExit(lateralSettings.largeError, lateralSettings.smallError, lateralSettings.largeErrorTimeout, lateralSettings.smallErrorTimeout, timeout);

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
            maxSpeed = (std::fabs(prevLateralPower) <  30) ? 30 : std::fabs(prevLateralPower);
        }

        // limit acceleration
        if (!close) lateralPower = lemlib::slew(lateralPower, prevLateralPower, lateralSettings.slew);
        if (std::fabs(angularError) > 25) angularPower = lemlib::slew(angularPower, prevAngularPower, angularSettings.slew);

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
