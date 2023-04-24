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
#include <algorithm>
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

    driveCurveScale = 0;
}

/**
 * @brief Calibrate the chassis sensors
 *
 */
void lemlib::Chassis::calibrate() {
    // calibrate the imu if it exists
    if (odomSensors.imu != nullptr) odomSensors.imu->reset(true);
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
 * @param log whether the chassis should log the turnTo function. false by default
 */
void lemlib::Chassis::turnTo(float x, float y, int timeout, bool reversed, float maxSpeed, bool log) {
    Pose pose(0, 0);
    float targetTheta;
    float deltaX, deltaY, deltaTheta;
    float motorPower;
    std::uint8_t compState = pros::competition::get_status();

    // create a new PID controller
    FAPID pid = FAPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    pid.setExit(angularSettings.largeError, angularSettings.smallError, angularSettings.largeErrorTimeout,
                angularSettings.smallErrorTimeout, timeout);

    // main loop
    while (pros::competition::get_status() == compState && !pid.settled()) {
        // update variables
        pose = getPose();
        pose.theta = (reversed) ? fmod(pose.theta - 180, 360) : fmod(pose.theta, 360);
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
void lemlib::Chassis::moveTo(float x, float y, int timeout, float maxSpeed, bool log) {
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

double calcDriveCurve(double input, double scale) {
    if (scale != 0) {
        return (powf(2.718, -(scale / 10)) + powf(2.718, (fabs(input) - 127) / 10) * (1 - powf(2.718, -(scale / 10)))) *
               input;
    }
    return input;
}

void lemlib::Chassis::setDriveCurveScale(double scale) { driveCurveScale = scale; }

// Credit to Okapilib for these implementations:
// https://github.com/purduesigbots/OkapiLib/blob/1355c29e1cc7a01dc2080434ecb7b32bec7b8328/src/api/chassis/model/skidSteerModel.cpp#L79

void lemlib::Chassis::tank(double leftSpeed, double rightSpeed, double deadzone) {
    leftSpeed = std::clamp(leftSpeed, -127.0, 127.0);
    if (std::abs(leftSpeed) <= deadzone) { leftSpeed = 0; }

    rightSpeed = std::clamp(rightSpeed, -127.0, 127.0);
    if (std::abs(rightSpeed) <= deadzone) { rightSpeed = 0; }

    drivetrain.leftMotors->move(calcDriveCurve(leftSpeed, 0));
    drivetrain.rightMotors->move(calcDriveCurve(rightSpeed, 0));
};

void lemlib::Chassis::arcade(double forwardSpeed, double yaw, double deadzone) {
    forwardSpeed = calcDriveCurve(forwardSpeed, driveCurveScale);
    forwardSpeed = std::clamp(forwardSpeed, -127.0, 127.0);
    if (std::abs(forwardSpeed) <= deadzone) { forwardSpeed = 0; }

    yaw = calcDriveCurve(yaw, driveCurveScale);
    yaw = std::clamp(yaw, -127.0, 127.0);
    if (std::abs(yaw) <= deadzone) { yaw = 0; }

    double maxInput = std::copysign(std::max(std::abs(forwardSpeed), std::abs(yaw)), forwardSpeed);

    double leftOutput = forwardSpeed + yaw;
    double rightOutput = forwardSpeed - yaw;

    if (forwardSpeed >= 0) {
        if (yaw >= 0) {
            leftOutput = maxInput;
            rightOutput = forwardSpeed - yaw;
        } else {
            leftOutput = forwardSpeed + yaw;
            rightOutput = maxInput;
        }
    } else {
        if (yaw >= 0) {
            leftOutput = forwardSpeed + yaw;
            rightOutput = maxInput;
        } else {
            leftOutput = maxInput;
            rightOutput = forwardSpeed - yaw;
        }
    }

    drivetrain.leftMotors->move(static_cast<int>(leftOutput));
    drivetrain.rightMotors->move(static_cast<int>(rightOutput));
}

void lemlib::Chassis::curvature(double iforwardSpeed, double icurvature, double deadzone) {
    double forwardSpeed = calcDriveCurve(iforwardSpeed, driveCurveScale);
    forwardSpeed /= 127;
    forwardSpeed = std::clamp(forwardSpeed, -1.0, 1.0);
    if (std::abs(forwardSpeed) < deadzone) { forwardSpeed = 0; }

    double curvature = calcDriveCurve(icurvature, driveCurveScale);
    curvature /= 127;
    curvature = std::clamp(curvature, -1.0, 1.0);
    if (std::abs(curvature) < deadzone) { curvature = 0; }

    // the algorithm switches to arcade when forward speed is 0 to allow point turns.
    if (forwardSpeed == 0) {
        arcade(iforwardSpeed, icurvature, deadzone);
        return;
    }

    double leftSpeed = forwardSpeed + std::abs(forwardSpeed) * curvature;
    double rightSpeed = forwardSpeed - std::abs(forwardSpeed) * curvature;
    double maxSpeed = std::max(leftSpeed, rightSpeed);

    // normalizes output
    if (maxSpeed > 1) {
        leftSpeed /= maxSpeed;
        rightSpeed /= maxSpeed;
    }

    drivetrain.leftMotors->move(static_cast<int>(leftSpeed * 127));
    drivetrain.rightMotors->move(static_cast<int>(rightSpeed * 127));
}
