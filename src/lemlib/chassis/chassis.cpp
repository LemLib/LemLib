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
    if (odomSensors.imu != nullptr) odomSensors.imu->reset(true);
    // initialize odom
    if (odomSensors.vertical1 != nullptr) odomSensors.vertical1->reset();
    if (odomSensors.vertical2 != nullptr) odomSensors.vertical2->reset();
    if (odomSensors.horizontal1 != nullptr) odomSensors.horizontal1->reset();
    if (odomSensors.horizontal2 != nullptr) odomSensors.horizontal2->reset();
    drivetrain.leftMotors->tare_position();
    drivetrain.rightMotors->tare_position();
    lemlib::setSensors(odomSensors, drivetrain);
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
    Pose pose(0, 0);
    float targetTheta;
    float deltaX, deltaY, deltaTheta;
    float motorPower;

    // create a new PID controller
    FAPID pid = FAPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
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
    Pose pose(0, 0);
    float directTheta, hypot, diffTheta, diffLateral, lateralPower, angularPower, leftPower, rightPower;
    bool close = false;

    // create a new PID controller
    FAPID lateralPID(0, 0, lateralSettings.kP, 0, lateralSettings.kD, "lateralPID");
    FAPID angularPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    lateralPID.setExit(lateralSettings.largeError, lateralSettings.smallError, lateralSettings.largeErrorTimeout, lateralSettings.smallErrorTimeout, timeout);

    // main loop
    while (pros::competition::is_autonomous() && !lateralPID.settled()) {
        // get the current position
        Pose pose = getPose(true);
        pose.theta = M_PI/2 - std::fmod(pose.theta, 360);

        // update error
        if (x == pose.x) pose.x += 0.000001;
        directTheta = atan2(y - pose.y, x - pose.x);
        hypot = std::hypot(x - pose.x, y - pose.y);
        diffTheta = pose.theta - directTheta;
        diffLateral = hypot * cos(diffTheta);

        // calculate the lateral speed
        lateralPower = lateralPID.update(diffLateral, 0, log);

        // calculate the angular speed
        // the robot can face the target heading in two ways, so we need to check which one is faster
        // these 2 ways are: facing the target heading directly, or 180 degrees away from the target heading
        // the robot will choose the faster way
        float diffTheta1 = radToDeg(angleError(pose.theta, directTheta, true));
        float diffTheta2 = radToDeg(angleError(pose.theta, directTheta + M_PI, true));
        // now decide which way is faster
        float diffTheta = (fabs(diffTheta1) < fabs(diffTheta2)) ? diffTheta1 : diffTheta2;
        
        angularPower = angularPID.update(diffTheta, 0, log);

        if (pose.distance(lemlib::Pose(x, y)) < 10) {
            close = true;
        }
        if (close) {
            angularPower = 0;
        } else {
            lateralPower *= std::fabs(std::cos(diffTheta));
        }

        // cap the speed
        if (lateralPower > maxSpeed) lateralPower = maxSpeed;
        else if (lateralPower < -maxSpeed) lateralPower = -maxSpeed;

        leftPower = lateralPower + angularPower;
        rightPower = lateralPower - angularPower;

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
