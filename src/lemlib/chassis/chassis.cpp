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
 * @brief Move the chassis to a target pose
 *
 * @param x x location
 * @param y y location
 * @param heading the heading, in degrees
 * @param timeout longest time the robot can spend moving
 * @param d the lead distance gain. 0 < d < 1. Default is 0.6
 * @param maxSpeed the maximum speed the robot can move at. Default is 127
 */
void lemlib::Chassis::moveTo(float x, float y, float heading, int timeout, float d, float maxSpeed) {
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
        // cap the speeds. This prevents one output dominating the other
        lateralPower = std::clamp(lateralPower, -maxSpeed, maxSpeed);
        angularPower = std::clamp(angularPower, -maxSpeed, maxSpeed);
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

    // stop the motors
    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
}
