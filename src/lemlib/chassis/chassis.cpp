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
#include "pros/motors.hpp"
#include "lemlib/util.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/odom.hpp"


/**
 * @brief Construct a new Chassis
 * 
 * @param leftMotors motors on the left side of the drivetrain
 * @param rightMotors motors on the right side of the drivetrain
 * @param sensors sensors to be used for odometry
 */
lemlib::Chassis::Chassis(pros::Motor_Group *leftMotors, pros::Motor_Group *rightMotors, OdomSensors_t sensors)
{
    leftMotorGroup = leftMotors;
    rightMotorGroup = rightMotors;
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