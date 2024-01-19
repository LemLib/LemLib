#include <algorithm>
#include <math.h>
#include <optional>
#include "lemlib/pid.hpp"
#include "pros/motors.hpp"
#include "pros/misc.hpp"
#include "pros/rtos.h"
#include "lemlib/util.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/odom.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "lemlib/timer.hpp"
#include "pros/rtos.hpp"

/**
 * @brief The variables are pointers so that they can be set to nullptr if they are not used
 * Otherwise the chassis class would have to have a constructor for each possible combination of sensors
 *
 * @param vertical1 pointer to the first vertical tracking wheel
 * @param vertical2 pointer to the second vertical tracking wheel
 * @param horizontal1 pointer to the first horizontal tracking wheel
 * @param horizontal2 pointer to the second horizontal tracking wheel
 * @param imu pointer to the IMU
 */
lemlib::OdomSensors::OdomSensors(TrackingWheel* vertical1, TrackingWheel* vertical2, TrackingWheel* horizontal1,
                                 TrackingWheel* horizontal2, pros::Imu* imu)
    : vertical1(vertical1),
      vertical2(vertical2),
      horizontal1(horizontal1),
      horizontal2(horizontal2),
      imu(imu) {}

/**
 * @brief The constants are stored in a struct so that they can be easily passed to the chassis class
 * Set a constant to 0 and it will be ignored
 *
 * @param leftMotors pointer to the left motors
 * @param rightMotors pointer to the right motors
 * @param trackWidth the track width of the robot
 * @param wheelDiameter the diameter of the wheel used on the drivetrain
 * @param rpm the rpm of the wheels
 * @param chasePower higher values make the robot move faster but causes more overshoot on turns
 */
lemlib::Drivetrain::Drivetrain(pros::MotorGroup* leftMotors, pros::MotorGroup* rightMotors, float trackWidth,
                               float wheelDiameter, float rpm, float chasePower)
    : leftMotors(leftMotors),
      rightMotors(rightMotors),
      trackWidth(trackWidth),
      wheelDiameter(wheelDiameter),
      rpm(rpm),
      chasePower(chasePower) {}

/**
 * @brief Construct a new Chassis
 *
 * @param drivetrain drivetrain to be used for the chassis
 * @param lateralSettings settings for the lateral controller
 * @param angularSettings settings for the angular controller
 * @param sensors sensors to be used for odometry
 * @param driveCurve drive curve to be used. defaults to `defaultDriveCurve`
 */
lemlib::Chassis::Chassis(Drivetrain drivetrain, ControllerSettings lateralSettings, ControllerSettings angularSettings,
                         OdomSensors sensors, DriveCurveFunction_t driveCurve)
    : drivetrain(drivetrain),
      lateralSettings(lateralSettings),
      angularSettings(angularSettings),
      sensors(sensors),
      driveCurve(driveCurve),
      lateralPID(lateralSettings.kP, lateralSettings.kI, lateralSettings.kD, lateralSettings.windupRange, true),
      angularPID(angularSettings.kP, angularSettings.kI, angularSettings.kD, angularSettings.windupRange, true),
      lateralLargeExit(lateralSettings.largeError, lateralSettings.largeErrorTimeout),
      lateralSmallExit(lateralSettings.smallError, lateralSettings.smallErrorTimeout),
      angularLargeExit(angularSettings.largeError, angularSettings.largeErrorTimeout),
      angularSmallExit(angularSettings.smallError, angularSettings.smallErrorTimeout) {}

/**
 * @brief Calibrate the chassis sensors
 *
 * @param calibrateIMU whether the IMU should be calibrated. true by default
 */
void lemlib::Chassis::calibrate(bool calibrateIMU) {
    // calibrate the IMU if it exists and the user doesn't specify otherwise
    if (sensors.imu != nullptr && calibrateIMU) {
        int attempt = 1;
        // calibrate inertial, and if calibration fails, then repeat 5 times or until successful
        while (sensors.imu->reset(true) != 1 && attempt < 5) {
            pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "---");
            pros::delay(10);
            attempt++;
        }
        if (attempt == 5) sensors.imu = nullptr;
    }
    // initialize odom
    if (sensors.vertical1 == nullptr)
        sensors.vertical1 = new lemlib::TrackingWheel(drivetrain.leftMotors, drivetrain.wheelDiameter,
                                                      -(drivetrain.trackWidth / 2), drivetrain.rpm);
    if (sensors.vertical2 == nullptr)
        sensors.vertical2 = new lemlib::TrackingWheel(drivetrain.rightMotors, drivetrain.wheelDiameter,
                                                      drivetrain.trackWidth / 2, drivetrain.rpm);
    sensors.vertical1->reset();
    sensors.vertical2->reset();
    if (sensors.horizontal1 != nullptr) sensors.horizontal1->reset();
    if (sensors.horizontal2 != nullptr) sensors.horizontal2->reset();
    lemlib::setSensors(sensors, drivetrain);
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
void lemlib::Chassis::setPose(float x, float y, float theta, bool radians) {
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
lemlib::Pose lemlib::Chassis::getPose(bool radians, bool standardPos) {
    Pose pose = lemlib::getPose(true);
    if (standardPos) pose.theta = M_PI_2 - pose.theta;
    if (!radians) pose.theta = radToDeg(pose.theta);
    return pose;
}

/**
 * @brief Wait until the robot has traveled a certain distance along the path
 *
 * @note Units are in inches if current motion is moveTo or follow, degrees if using turnTo
 *
 * @param dist the distance the robot needs to travel before returning
 */
void lemlib::Chassis::waitUntil(float dist) {
    // do while to give the thread time to start
    do pros::delay(10);
    while (distTravelled <= dist && distTravelled != -1);
}

/**
 * @brief Wait until the robot has completed the path
 *
 */
void lemlib::Chassis::waitUntilDone() {
    do pros::delay(10);
    while (distTravelled != -1);
}

void lemlib::Chassis::requestMotionStart() {
    if (this->isInMotion()) this->motionQueued = true; // indicate a motion is queued
    else this->motionRunning = true; // indicate a motion is running

    // wait until this motion is at front of "queue"
    this->mutex.take(TIMEOUT_MAX);

    // this->motionRunning should be true
    // and this->motionQueued should be false
    // indicating this motion is running
}

void lemlib::Chassis::endMotion() {
    // move the "queue" forward 1
    this->motionRunning = this->motionQueued;
    this->motionQueued = false;

    // permit queued motion to run
    this->mutex.give();
}

void lemlib::Chassis::cancelMotion() {
    this->motionRunning = false;
    pros::delay(10); // give time for motion to stop
}

void lemlib::Chassis::cancelAllMotions() {
    this->motionRunning = false;
    this->motionQueued = false;
    pros::delay(10); // give time for motion to stop
}

bool lemlib::Chassis::isInMotion() const { return this->motionRunning; }

/**
 * @brief Sets the brake mode of the drivetrain motors
 *
 */
void lemlib::Chassis::setBrakeMode(pros::motor_brake_mode_e mode) {
    drivetrain.leftMotors->set_brake_modes(mode);
    drivetrain.rightMotors->set_brake_modes(mode);
}

/**
 * @brief Turn the chassis so it is facing the target point
 *
 * The PID logging id is "angularPID"
 *
 * @param x x location
 * @param y y location
 * @param timeout longest time the robot can spend moving
 * @param params struct to simulate named parameters
 * @param async whether the function should be run asynchronously. true by default
 */
void lemlib::Chassis::turnTo(float x, float y, int timeout, TurnToParams params, bool async) {
    params.minSpeed = fabs(params.minSpeed);
    this->requestMotionStart();
    // were all motions cancelled?
    if (!this->motionRunning) return;
    // if the function is async, run it in a new task
    if (async) {
        pros::Task task([&]() { turnTo(x, y, timeout, params, false); });
        this->endMotion();
        pros::delay(10); // delay to give the task time to start
        return;
    }
    float targetTheta;
    float deltaX, deltaY, deltaTheta;
    float motorPower;
    float prevMotorPower = 0;
    float startTheta = getPose().theta;
    std::optional<float> prevDeltaTheta = std::nullopt;
    std::uint8_t compState = pros::competition::get_status();
    distTravelled = 0;
    Timer timer(timeout);
    angularLargeExit.reset();
    angularSmallExit.reset();
    angularPID.reset();

    // main loop
    while (!timer.isDone() && !angularLargeExit.getExit() && !angularSmallExit.getExit() && this->motionRunning) {
        // update variables
        Pose pose = getPose();
        pose.theta = (params.forwards) ? fmod(pose.theta, 360) : fmod(pose.theta - 180, 360);

        // update completion vars
        distTravelled = fabs(angleError(pose.theta, startTheta));

        deltaX = x - pose.x;
        deltaY = y - pose.y;
        targetTheta = fmod(radToDeg(M_PI_2 - atan2(deltaY, deltaX)), 360);

        // calculate deltaTheta
        deltaTheta = angleError(targetTheta, pose.theta, false);
        if (prevDeltaTheta == std::nullopt) prevDeltaTheta = deltaTheta;

        // motion chaining
        if (params.minSpeed != 0 && fabs(deltaTheta) < params.earlyExitRange) break;
        if (params.minSpeed != 0 && sgn(deltaTheta) != sgn(prevDeltaTheta)) break;

        // calculate the speed
        motorPower = angularPID.update(deltaTheta);
        angularLargeExit.update(deltaTheta);
        angularSmallExit.update(deltaTheta);

        // cap the speed
        if (motorPower > params.maxSpeed) motorPower = params.maxSpeed;
        else if (motorPower < -params.maxSpeed) motorPower = -params.maxSpeed;
        if (fabs(deltaTheta) > 20) motorPower = slew(motorPower, prevMotorPower, angularSettings.slew);
        if (motorPower < 0 && motorPower > -params.minSpeed) motorPower = -params.minSpeed;
        else if (motorPower > 0 && motorPower < params.minSpeed) motorPower = params.minSpeed;
        prevMotorPower = motorPower;

        // move the drivetrain
        drivetrain.leftMotors->move(motorPower);
        drivetrain.rightMotors->move(-motorPower);

        pros::delay(10);
    }

    // stop the drivetrain
    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
    // set distTraveled to -1 to indicate that the function has finished
    distTravelled = -1;
    this->endMotion();
}

/**
 * @brief Move the chassis towards the target pose
 *
 * Uses the boomerang controller
 *
 * @param x x location
 * @param y y location
 * @param theta target heading in degrees.
 * @param timeout longest time the robot can spend moving
 *
 * @param maxSpeed the maximum speed the robot can move at. 127 at default
 * @param async whether the function should be run asynchronously. true by default
 */
void lemlib::Chassis::moveToPose(float x, float y, float theta, int timeout, MoveToPoseParams params, bool async) {
    // take the mutex
    this->requestMotionStart();
    // were all motions cancelled?
    if (!this->motionRunning) return;
    // if the function is async, run it in a new task
    if (async) {
        pros::Task task([&]() { moveToPose(x, y, theta, timeout, params, false); });
        this->endMotion();
        pros::delay(10); // delay to give the task time to start
        return;
    }

    // reset PIDs and exit conditions
    lateralPID.reset();
    lateralLargeExit.reset();
    lateralSmallExit.reset();
    angularPID.reset();
    angularLargeExit.reset();
    angularSmallExit.reset();

    // calculate target pose in standard form
    Pose target(x, y, M_PI_2 - degToRad(theta));
    if (!params.forwards) target.theta = fmod(target.theta + M_PI, 2 * M_PI); // backwards movement

    // use global chasePower is chasePower is 0
    if (params.chasePower == 0) params.chasePower = drivetrain.chasePower;

    // initialize vars used between iterations
    Pose lastPose = getPose();
    distTravelled = 0;
    Timer timer(timeout);
    bool close = false;
    bool lateralSettled = false;
    bool prevSameSide = false;
    float prevLateralOut = 0; // previous lateral power
    float prevAngularOut = 0; // previous angular power
    const int compState = pros::competition::get_status();

    // main loop
    while (!timer.isDone() &&
           ((!lateralSettled || (!angularLargeExit.getExit() && !angularSmallExit.getExit())) || !close) &&
           this->motionRunning) {
        // update position
        const Pose pose = getPose(true, true);

        // update distance travelled
        distTravelled += pose.distance(lastPose);
        lastPose = pose;

        // calculate distance to the target point
        const float distTarget = pose.distance(target);

        // check if the robot is close enough to the target to start settling
        if (distTarget < 7.5 && close == false) {
            close = true;
            params.maxSpeed = fmax(fabs(prevLateralOut), 60);
        }

        // check if the lateral controller has settled
        if (lateralLargeExit.getExit() && lateralSmallExit.getExit()) lateralSettled = true;

        // calculate the carrot point
        Pose carrot = target - Pose(cos(target.theta), sin(target.theta)) * params.lead * distTarget;
        if (close) carrot = target; // settling behavior

        // calculate if the robot is on the same side as the carrot point
        const bool robotSide =
            (pose.y - target.y) * -sin(target.theta) <= (pose.x - target.x) * cos(target.theta) + params.earlyExitRange;
        const bool carrotSide = (carrot.y - target.y) * -sin(target.theta) <=
                                (carrot.x - target.x) * cos(target.theta) + params.earlyExitRange;
        const bool sameSide = robotSide == carrotSide;
        // exit if close
        if (!sameSide && prevSameSide && close && params.minSpeed != 0) break;
        prevSameSide = sameSide;

        // calculate error
        const float adjustedRobotTheta = params.forwards ? pose.theta : pose.theta + M_PI;
        const float angularError =
            close ? angleError(adjustedRobotTheta, target.theta) : angleError(adjustedRobotTheta, pose.angle(carrot));
        float lateralError = pose.distance(carrot);
        // only use cos when settling
        // otherwise just multiply by the sign of cos
        // maxSlipSpeed takes care of lateralOut
        if (close) lateralError *= cos(angleError(pose.theta, pose.angle(carrot)));
        else lateralError *= sgn(cos(angleError(pose.theta, pose.angle(carrot))));

        // update exit conditions
        lateralSmallExit.update(lateralError);
        lateralLargeExit.update(lateralError);
        angularSmallExit.update(radToDeg(angularError));
        angularLargeExit.update(radToDeg(angularError));

        // get output from PIDs
        float lateralOut = lateralPID.update(lateralError);
        float angularOut = angularPID.update(radToDeg(angularError));

        // apply restrictions on angular speed
        angularOut = std::clamp(angularOut, -params.maxSpeed, params.maxSpeed);

        // apply restrictions on lateral speed
        lateralOut = std::clamp(lateralOut, -params.maxSpeed, params.maxSpeed);

        // constrain lateral output by max accel
        if (!close) lateralOut = slew(lateralOut, prevLateralOut, lateralSettings.slew);

        // constrain lateral output by the max speed it can travel at without slipping
        const float radius = 1 / fabs(getCurvature(pose, carrot));
        const float maxSlipSpeed(sqrt(params.chasePower * radius * 9.8));
        lateralOut = std::clamp(lateralOut, -maxSlipSpeed, maxSlipSpeed);
        // prioritize angular movement over lateral movement
        const float overturn = fabs(angularOut) + fabs(lateralOut) - params.maxSpeed;
        if (overturn > 0) lateralOut -= lateralOut > 0 ? overturn : -overturn;

        // prevent moving in the wrong direction
        if (params.forwards && !close) lateralOut = std::fmax(lateralOut, 0);
        else if (!params.forwards && !close) lateralOut = std::fmin(lateralOut, 0);

        // constrain lateral output by the minimum speed
        if (params.forwards && lateralOut < fabs(params.minSpeed) && lateralOut > 0) lateralOut = fabs(params.minSpeed);
        if (!params.forwards && -lateralOut < fabs(params.minSpeed) && lateralOut < 0)
            lateralOut = -fabs(params.minSpeed);

        // update previous output
        prevAngularOut = angularOut;
        prevLateralOut = lateralOut;

        // ratio the speeds to respect the max speed
        float leftPower = lateralOut + angularOut;
        float rightPower = lateralOut - angularOut;
        const float ratio = std::max(std::fabs(leftPower), std::fabs(rightPower)) / params.maxSpeed;
        if (ratio > 1) {
            leftPower /= ratio;
            rightPower /= ratio;
        }

        // move the drivetrain
        drivetrain.leftMotors->move(leftPower);
        drivetrain.rightMotors->move(rightPower);

        // delay to save resources
        pros::delay(10);
    }

    // stop the drivetrain
    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
    // set distTraveled to -1 to indicate that the function has finished
    distTravelled = -1;
    this->endMotion();
}

/**
 * @brief Move the chassis towards a target point
 *
 * @param x x location
 * @param y y location
 * @param timeout longest time the robot can spend moving
 * @param params struct to simulate named parameters
 * @param async whether the function should be run asynchronously. true by default
 */
void lemlib::Chassis::moveToPoint(float x, float y, int timeout, MoveToPointParams params, bool async) {
    params.earlyExitRange = fabs(params.earlyExitRange);
    this->requestMotionStart();
    // were all motions cancelled?
    if (!this->motionRunning) return;
    // if the function is async, run it in a new task
    if (async) {
        pros::Task task([&]() { moveToPoint(x, y, timeout, params, false); });
        this->endMotion();
        pros::delay(10); // delay to give the task time to start
        return;
    }

    // reset PIDs and exit conditions
    lateralPID.reset();
    lateralLargeExit.reset();
    lateralSmallExit.reset();
    angularPID.reset();

    // initialize vars used between iterations
    Pose lastPose = getPose();
    distTravelled = 0;
    Timer timer(timeout);
    bool close = false;
    float prevLateralOut = 0; // previous lateral power
    float prevAngularOut = 0; // previous angular power
    const int compState = pros::competition::get_status();
    std::optional<bool> prevSide = std::nullopt;

    // calculate target pose in standard form
    Pose target(x, y);
    target.theta = lastPose.angle(target);

    // main loop
    while (!timer.isDone() && ((!lateralSmallExit.getExit() && !lateralLargeExit.getExit()) || !close) &&
           this->motionRunning) {
        // update position
        const Pose pose = getPose(true, true);

        // update distance travelled
        distTravelled += pose.distance(lastPose);
        lastPose = pose;

        // calculate distance to the target point
        const float distTarget = pose.distance(target);

        // check if the robot is close enough to the target to start settling
        if (distTarget < 7.5 && close == false) {
            close = true;
            params.maxSpeed = fmax(fabs(prevLateralOut), 60);
        }

        // motion chaining
        const bool side =
            (pose.y - target.y) * -sin(target.theta) <= (pose.x - target.x) * cos(target.theta) + params.earlyExitRange;
        if (prevSide == std::nullopt) prevSide = side;
        const bool sameSide = side == prevSide;
        // exit if close
        if (!sameSide && params.minSpeed != 0) break;
        prevSide = side;

        // calculate error
        const float adjustedRobotTheta = params.forwards ? pose.theta : pose.theta + M_PI;
        const float angularError = angleError(adjustedRobotTheta, pose.angle(target));
        float lateralError = pose.distance(target) * cos(angleError(pose.theta, pose.angle(target)));

        // update exit conditions
        lateralSmallExit.update(lateralError);
        lateralLargeExit.update(lateralError);

        // get output from PIDs
        float lateralOut = lateralPID.update(lateralError);
        float angularOut = angularPID.update(radToDeg(angularError));
        if (close) angularOut = 0;

        // apply restrictions on angular speed
        angularOut = std::clamp(angularOut, -params.maxSpeed, params.maxSpeed);
        angularOut = slew(angularOut, prevAngularOut, angularSettings.slew);

        // apply restrictions on lateral speed
        lateralOut = std::clamp(lateralOut, -params.maxSpeed, params.maxSpeed);
        // constrain lateral output by max accel
        // but not for decelerating, since that would interfere with settling
        if (!close) lateralOut = slew(lateralOut, prevLateralOut, lateralSettings.slew);

        // prevent moving in the wrong direction
        if (params.forwards && !close) lateralOut = std::fmax(lateralOut, 0);
        else if (!params.forwards && !close) lateralOut = std::fmin(lateralOut, 0);

        // constrain lateral output by the minimum speed
        if (params.forwards && lateralOut < fabs(params.minSpeed) && lateralOut > 0) lateralOut = fabs(params.minSpeed);
        if (!params.forwards && -lateralOut < fabs(params.minSpeed) && lateralOut < 0)
            lateralOut = -fabs(params.minSpeed);

        // update previous output
        prevAngularOut = angularOut;
        prevLateralOut = lateralOut;

        // ratio the speeds to respect the max speed
        float leftPower = lateralOut + angularOut;
        float rightPower = lateralOut - angularOut;
        const float ratio = std::max(std::fabs(leftPower), std::fabs(rightPower)) / params.maxSpeed;
        if (ratio > 1) {
            leftPower /= ratio;
            rightPower /= ratio;
        }

        // move the drivetrain
        drivetrain.leftMotors->move(leftPower);
        drivetrain.rightMotors->move(rightPower);

        // delay to save resources
        pros::delay(10);
    }

    // stop the drivetrain
    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
    // set distTraveled to -1 to indicate that the function has finished
    distTravelled = -1;
    this->endMotion();
}

/**
 * @brief Get the lateral PID gains
 *
 * @return gains the current PID gains
 */
lemlib::Gains lemlib::Chassis::getLateralGains() { return this->lateralPID.getGains(); }

/**
 * @brief Get the angular PID gains
 *
 * @return gains the current PID gains
 */
lemlib::Gains lemlib::Chassis::getAngularGains() { return this->angularPID.getGains(); }

/**
 * @brief Set the lateral PID gains
 *
 * @param gains the new PID gains
 */
void lemlib::Chassis::setLateralGains(lemlib::GainOptions gains) { this->lateralPID.setGains(gains); }

/**
 * @brief Set the angular PID gains
 *
 * @param gains the new PID gains
 */
void lemlib::Chassis::setAngularGains(lemlib::GainOptions gains) { this->angularPID.setGains(gains); }