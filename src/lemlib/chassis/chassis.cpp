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
#include "lemlib/chassis/trackingWheel.hpp"
#include "lemlib/logger.hpp"

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
 * @param preservePose true if the pose should be preserved, false if not. False by default
 */
void lemlib::Chassis::calibrate(bool preservePose) {
    logger::debug("Calibrating chassis...");

    // initialize tracking wheels
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

    // get the competition state
    // if it changes to anything other than disabled, abandon the calibration
    uint8_t compState = pros::competition::get_status();
    if (!preservePose) pose = lemlib::Pose(0, 0, 0);
    odomMutex.take(TIMEOUT_MAX);
    // calibrate the imu if it exists
    if (odomSensors.imu != nullptr) {
        bool imuCalibrated = false;
        do {
            int32_t imuReturn = odomSensors.imu->reset();
            // check if the imu reset failed loudly
            if (imuReturn == PROS_ERR) {
                logger::error("IMU calibration failed: IMU returned PROS_ERR. Retrying...");
                pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "---");
                pros::delay(10);
                continue;
            }
            // wait for the imu to calibrate
            while (odomSensors.imu->is_calibrating()) {
                // exit if the competition state changes to anything other than disabled
                if (pros::competition::get_status() != compState && !pros::competition::is_disabled()) {
                    logger::error("IMU calibration failed: competition state changed. Disabling IMU...");
                    pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "---");
                    odomSensors.imu = nullptr; // disable the imu
                    // start the odom task
                    odomMutex.give();
                    if (odomTask == nullptr) odomTask = new pros::Task {[=] { odom(); }};
                    // exit the function
                    return;
                }
                // wait and check again
                pros::delay(10);
            }
            // check if the imu reset failed quietly
            double imuTestRotation = odomSensors.imu->get_rotation();
            if (std::isinf(imuTestRotation) || std::isnan(imuTestRotation)) {
                logger::error("IMU calibration failed: IMU returned NaN or Inf. Retrying...");
                pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "---");
                pros::delay(10);
                continue;
            }
            // imu calibrated successfully
            imuCalibrated = true;
        } while (!imuCalibrated);
    }

    // start the odom task
    odomMutex.give();
    if (odomTask == nullptr) odomTask = new pros::Task {[=] { odom(); }};
    // rumble to controller to indicate success
    pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, ".");
    logger::debug("Chassis calibrated");
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
    if (radians) Chassis::pose = lemlib::Pose(x, y, theta);
    else Chassis::pose = lemlib::Pose(x, y, util::degToRad(theta));
}

/**
 * @brief Set the Pose of the robot
 *
 * @param pose the new pose
 * @param radians true if theta is in radians, false if in degrees. False by default
 */
void lemlib::Chassis::setPose(lemlib::Pose pose, bool radians) {
    if (radians) Chassis::pose = pose;
    else Chassis::pose = lemlib::Pose(pose.x, pose.y, util::degToRad(pose.theta));
}

/**
 * @brief Get the pose of the robot
 *
 * @param radians true for theta in radians, false for degrees. False by default
 * @return Pose
 */
lemlib::Pose lemlib::Chassis::getPose(bool radians) {
    if (radians) return pose;
    else return lemlib::Pose(pose.x, pose.y, util::radToDeg(pose.theta));
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
void lemlib::Chassis::turnTo(float x, float y, int timeout, bool reversed, float maxSpeed, bool log) {
    logger::debug("Turning to face (%f, %f)", x, y);
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
        pose = getPose();
        pose.theta = (reversed) ? fmod(pose.theta - 180, 360) : fmod(pose.theta, 360);
        deltaX = x - pose.x;
        deltaY = y - pose.y;
        targetTheta = fmod(util::radToDeg(M_PI_2 - atan2(deltaY, deltaX)), 360);
        deltaTheta = util::angleError(targetTheta, pose.theta);
        motorPower = pid.update(0, deltaTheta, log);

        // cap the speed
        if (motorPower > maxSpeed) motorPower = maxSpeed;
        else if (motorPower < -maxSpeed) motorPower = -maxSpeed;

        drivetrain.leftMotors->move(-motorPower);
        drivetrain.rightMotors->move(motorPower);

        pros::delay(10);
    }
    logger::debug("Finished turning to (%f, %f)", x, y);
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
    logger::debug("Moving to (%f, %f)", x, y);
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
        float targetTheta = fmod(util::radToDeg(M_PI_2 - atan2(deltaY, deltaX)), 360);
        float hypot = std::hypot(deltaX, deltaY);
        float diffTheta1 = util::angleError(pose.theta, targetTheta);
        float diffTheta2 = util::angleError(pose.theta, targetTheta + 180);
        float angularError = (std::fabs(diffTheta1) < std::fabs(diffTheta2)) ? diffTheta1 : diffTheta2;
        float lateralError = hypot * cos(util::degToRad(std::fabs(diffTheta1)));

        // calculate speed
        float lateralPower = lateralPID.update(lateralError, 0, log);
        float angularPower = -angularPID.update(angularError, 0, log);

        // if the robot is close to the target
        if (pose.distance(lemlib::Pose(x, y)) < 7.5) {
            close = true;
            maxSpeed = (std::fabs(prevLateralPower) < 30) ? 30 : std::fabs(prevLateralPower);
        }

        // limit acceleration
        if (!close) lateralPower = util::slew(lateralPower, prevLateralPower, lateralSettings.slew);
        if (std::fabs(angularError) > 25)
            angularPower = util::slew(angularPower, prevAngularPower, angularSettings.slew);

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

    logger::debug("Finished moving to (%f, %f)", x, y);
    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
}

/**
 * @brief Odometry update function
 *
 */
void lemlib::Chassis::odom() {
    logger::debug("Starting odometry");
    float prevVertical = 0;
    float prevVertical1 = 0;
    float prevVertical2 = 0;
    float prevHorizontal = 0;
    float prevHorizontal1 = 0;
    float prevHorizontal2 = 0;
    float prevImu = 0;
    float vertical1Raw = 0;
    float vertical2Raw = 0;
    float horizontal1Raw = 0;
    float horizontal2Raw = 0;
    float imuRaw = 0;

    // loop forever
    while (true) {
        // try to take the odom mutex
        if (!odomMutex.take(100)) {
            // if we can't, this means the chassis is being calibrated
            // reset previous values and wait
            prevVertical = 0;
            prevVertical1 = 0;
            prevVertical2 = 0;
            prevHorizontal = 0;
            prevHorizontal1 = 0;
            prevHorizontal2 = 0;
            prevImu = 0;
            pros::delay(10);
            continue;
        }

        // see what sensors are available
        if (odomSensors.vertical1 != nullptr) vertical1Raw = odomSensors.vertical1->getDistanceTraveled();
        if (odomSensors.vertical2 != nullptr) vertical2Raw = odomSensors.vertical2->getDistanceTraveled();
        if (odomSensors.horizontal1 != nullptr) horizontal1Raw = odomSensors.horizontal1->getDistanceTraveled();
        if (odomSensors.horizontal2 != nullptr) horizontal2Raw = odomSensors.horizontal2->getDistanceTraveled();
        // use the IMU, if it returns a valid heading
        if (odomSensors.imu != nullptr && !isinf(odomSensors.imu->get_rotation())) {
            imuRaw = util::degToRad(odomSensors.imu->get_rotation());
        } else if (isinf(odomSensors.imu->get_rotation())) { // otherwise, take action
            logger::error("IMU returned inf, taking action...");
            if (pros::competition::is_disabled()) { // robot is disabled, we have time to reset
                logger::warn("Resetting IMU");
                this->calibrate(true);
            } else { // robot is active, disable the IMU
                logger::warn("Disabling IMU");
                odomSensors.imu = nullptr;
            }
        }

        // calculate deltas
        float deltaVertical1 = vertical1Raw - prevVertical1;
        float deltaVertical2 = vertical2Raw - prevVertical2;
        float deltaHorizontal1 = horizontal1Raw - prevHorizontal1;
        float deltaHorizontal2 = horizontal2Raw - prevHorizontal2;
        float deltaImu = imuRaw - prevImu;
        prevVertical1 = vertical1Raw;
        prevVertical2 = vertical2Raw;
        prevHorizontal1 = horizontal1Raw;
        prevHorizontal2 = horizontal2Raw;
        prevImu = imuRaw;

        // calculate the heading of the robot
        // Priority:
        // 1. Horizontal tracking wheels
        // 2. Vertical tracking wheels
        // 3. Inertial Sensor
        // 4. Drivetrain
        float heading = pose.theta;
        // calculate the heading using the horizontal tracking wheels
        if (odomSensors.horizontal1 != nullptr && odomSensors.horizontal2 != nullptr)
            heading += (deltaHorizontal1 - deltaHorizontal2) /
                       (odomSensors.horizontal1->getOffset() - odomSensors.horizontal2->getOffset());
        // else, if both vertical tracking wheels aren't substituted by the drivetrain, use the vertical tracking wheels
        else if (odomSensors.vertical1->getType() == TrackingWheel::Type::POD &&
                 odomSensors.vertical2->getType() == TrackingWheel::Type::POD)
            heading += (deltaVertical1 - deltaVertical2) /
                       (odomSensors.vertical1->getOffset() - odomSensors.vertical2->getOffset());
        // else, if the inertial sensor exists, use it
        else if (odomSensors.imu != nullptr) heading += deltaImu;
        // else, use the the substituted tracking wheels
        else
            heading += (deltaVertical1 - deltaVertical2) /
                       (odomSensors.vertical1->getOffset() - odomSensors.vertical2->getOffset());
        float deltaHeading = heading - pose.theta;
        float avgHeading = pose.theta + deltaHeading / 2;

        // choose tracking wheels to use
        lemlib::TrackingWheel* verticalWheel = nullptr;
        lemlib::TrackingWheel* horizontalWheel = nullptr;
        if (odomSensors.vertical1->getType() == TrackingWheel::Type::POD) verticalWheel = odomSensors.vertical1;
        else if (odomSensors.vertical2->getType() == TrackingWheel::Type::POD) verticalWheel = odomSensors.vertical2;
        else verticalWheel = odomSensors.vertical1;
        if (odomSensors.horizontal1 != nullptr) horizontalWheel = odomSensors.horizontal1;
        else if (odomSensors.horizontal2 != nullptr) horizontalWheel = odomSensors.horizontal2;
        float rawVertical = 0;
        float rawHorizontal = 0;
        if (verticalWheel != nullptr) rawVertical = verticalWheel->getDistanceTraveled();
        if (horizontalWheel != nullptr) rawHorizontal = horizontalWheel->getDistanceTraveled();
        float horizontalOffset = 0;
        float verticalOffset = 0;
        if (verticalWheel != nullptr) verticalOffset = verticalWheel->getOffset();
        if (horizontalWheel != nullptr) horizontalOffset = horizontalWheel->getOffset();

        // calculate change in x and y
        float deltaX = 0;
        float deltaY = 0;
        if (verticalWheel != nullptr) deltaY = rawVertical - prevVertical;
        if (horizontalWheel != nullptr) deltaX = rawHorizontal - prevHorizontal;
        prevVertical = rawVertical;
        prevHorizontal = rawHorizontal;

        // calculate local x and y
        float localX = 0;
        float localY = 0;
        if (deltaHeading == 0) {
            localX = deltaX;
            localY = deltaY;
        } else {
            localX = 2 * sin(deltaHeading / 2) * (deltaX / deltaHeading + horizontalOffset);
            localY = 2 * sin(deltaHeading / 2) * (deltaY / deltaHeading + verticalOffset);
        }

        // calculate global x and y
        pose.x += localY * sin(avgHeading);
        pose.y += localY * cos(avgHeading);
        pose.x += localX * -cos(avgHeading);
        pose.y += localX * sin(avgHeading);
        pose.theta = heading;

        // check if the pose is valid
        if (!pose.isValid()) {
            logger::error("Odometry error. Did a tracking wheel disconnect?");
            return;
        }

        odomMutex.give();
        pros::delay(10);
    }
}
