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
#include <math.h>
#include "pros/motors.hpp"
#include "pros/misc.hpp"
#include "lemlib/util.hpp"
#include "lemlib/pid.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/odom.hpp"
#include <iostream>


/**
 * @brief Construct a new Chassis
 * 
 * @param leftMotors motors on the left side of the drivetrain
 * @param rightMotors motors on the right side of the drivetrain
 * @param lateralSettings settings for the lateral controller
 * @param angularSetting settings for the angular controller
 * @param topSpeed the top speed of the chassis. in/s
 * @param sensors sensors to be used for odometry
 */
lemlib::Chassis::Chassis(pros::Motor_Group *leftMotors, pros::Motor_Group *rightMotors, float topSpeed, ChassisController_t lateralSettings, ChassisController_t angularSetting, OdomSensors_t sensors)
{
    leftMotorGroup = leftMotors;
    rightMotorGroup = rightMotors;
    this->topSpeed = topSpeed;
    this->lateralSettings = lateralSettings;
    this->angularSettings = angularSetting;
    odomSensors = sensors;
}


/**
 * @brief Calibrate the chassis sensors
 * 
 */
void lemlib::Chassis::calibrate()
{
    // calibrate the imu if it exists
    if (odomSensors.imu != nullptr) odomSensors.imu->reset(true);
    // initialize odom
    lemlib::setSensors(odomSensors);
    lemlib::init();
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
 * @brief Move the chassis as close as possible to the target point in a straight line
 * 
 * @param x x location
 * @param y y location
 * @param timeout longest time the robot can spend moving
 * @param reversed whether the robot should turn in the opposite direction. false by default
 */
void lemlib::Chassis::turnTo(float x, float y, int timeout, bool reversed)
{
    Pose pose(0, 0);
    float targetTheta;
    float deltaX, deltaY, deltaTheta;
    float motorPower;

    // create a new PID controller
    FAPID pid = FAPID(0, angularSettings.kA, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    pid.setExit(angularSettings.largeError, angularSettings.smallError, angularSettings.largeErrorTimeout, angularSettings.smallErrorTimeout, timeout);

    // main loop
    while (pros::competition::is_autonomous() && !pid.settled()) {
        // update variables
        pose = getPose();
        pose.theta = (reversed) ? fmod(pose.theta-180, 360) : fmod(pose.theta, 360);
        deltaX = x - pose.x;
        // if deltaX is 0, set it to 0.000001 to prevent division by 0 by the atan2 function
        if (!deltaX) deltaX = 0.000001;
        deltaY = y - pose.y;
        targetTheta = fmod(radToDeg(M_PI/2 - atan2(deltaY, deltaX)), 360);
        
        // calculate deltaTheta
        deltaTheta = angleError(targetTheta, pose.theta);

        // calculate the speed
        motorPower = pid.update(0, deltaTheta);

        // move the drivetrain
        leftMotorGroup->move(-motorPower);
        rightMotorGroup->move(motorPower);

        pros::delay(10);
    }

    // stop the drivetrain
    leftMotorGroup->move(0);
    rightMotorGroup->move(0);
}


/**
 * @brief Move the chassis as close as possible to the target point in a straight line
 * 
 * @param x x location
 * @param y y location
 * @param timeout longest time the robot can spend moving
 * @param reversed whether the robot should turn in the opposite direction. false by default
 */
void lemlib::Chassis::moveTo(float x, float y, int timeout)
{
    Pose pose(0, 0);
    float directTheta, hypot, diffTheta, diffLateral, motorPower;

    // create a new PID controller
    FAPID pid(0, lateralSettings.kA, lateralSettings.kP, 0, lateralSettings.kD, "lateralPID");
    pid.setExit(lateralSettings.largeError, lateralSettings.smallError, lateralSettings.largeErrorTimeout, lateralSettings.smallErrorTimeout, timeout);

    // main loop
    while (pros::competition::is_autonomous() && !pid.settled()) {
        // get the current position
        Pose pose = getPose(true);
        pose.theta = M_PI/2 - pose.theta;

        // update error
        if (x == pose.x) pose.x += 0.000001;
        directTheta = atan2(y - pose.y, x - pose.x);
        hypot = std::hypot(x - pose.x, y - pose.y);
        diffTheta = pose.theta - directTheta;

        // calculate the speed
        motorPower = pid.update(hypot, 0) * std::cos(diffTheta);

        // move the motors
        leftMotorGroup->move(-motorPower);
        rightMotorGroup->move(-motorPower);

        pros::delay(10);
    }

    // stop the drivetrain
    leftMotorGroup->move(0);
    rightMotorGroup->move(0);
}
