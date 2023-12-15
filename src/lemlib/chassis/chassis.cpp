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
#include "pros/rtos.h"
#include "lemlib/util.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/odom.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "lemlib/timer.hpp"

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
        while (sensors.imu->reset(true) != 1 && (errno == PROS_ERR || errno == ENODEV || errno == ENXIO) &&
               attempt < 5) {
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
    Pose pose = lemlib::getPose(radians);
    pose.theta = (radians) ? M_PI_2 - pose.theta : 90 - pose.theta;
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
 * @param forwards whether the robot should turn to face the point with the front of the robot. true by default
 * @param maxSpeed the maximum speed the robot can turn at. Default is 127
 * @param async whether the function should be run asynchronously. true by default
 */
void lemlib::Chassis::turnTo(float x, float y, int timeout, bool forwards, float maxSpeed, bool async) {
    // take the mutex
    mutex.take(TIMEOUT_MAX);
    // if the function is async, run it in a new task
    if (async) {
        pros::Task task([&]() { turnTo(x, y, timeout, forwards, maxSpeed, false); });
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
        pose.theta = (forwards) ? fmod(pose.theta, 360) : fmod(pose.theta - 180, 360);

        // update completion vars
        distTravelled = fabs(angleError(pose.theta, startTheta));

        deltaX = x - pose.x;
        deltaY = y - pose.y;
        targetTheta = fmod(radToDeg(M_PI_2 - atan2(deltaY, deltaX)), 360);

        // calculate deltaTheta
        deltaTheta = angleError(targetTheta, pose.theta);

        // calculate the speed
        motorPower = pid.update(0, deltaTheta);

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

double maxSpeed(double curvature) {}

/**
 * @brief Move the chassis towards the target pose
 *
 * Uses the boomerang controller
 *
 * @param x x location
 * @param y y location
 * @param theta target heading in degrees.
 * @param timeout longest time the robot can spend moving
 * @param forwards whether the robot should move forwards or backwards. true for forwards (default), false for
 * backwards
 * @param chasePower higher values make the robot move faster but causes more overshoot on turns. 0 makes it
 * default to global value
 * @param lead the lead parameter. Determines how curved the robot will move. 0.6 by default (0 < lead < 1)
 * @param maxSpeed the maximum speed the robot can move at. 127 at default
 * @param async whether the function should be run asynchronously. true by default
 */
void lemlib::Chassis::moveToPose(float x, float y, float theta, int timeout, bool forwards, float chasePower,
                                 float lead, float maxSpeed, bool async) {
    // take the mutex
    mutex.take(TIMEOUT_MAX);
    // if the function is async, run it in a new task
    if (async) {
        pros::Task task([&]() { moveToPose(x, y, theta, timeout, forwards, chasePower, lead, maxSpeed, false); });
        mutex.give(); // release the mutex so the lambda can run
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
    if (!forwards) target.theta = fmod(target.theta + M_PI, 2 * M_PI); // backwards movement

    // use global chasePower is chasePower is 0
    if (chasePower == 0) chasePower = drivetrain.chasePower;

    // initialize vars used between iterations
    Pose lastPose = getPose();
    distTravelled = 0;
    Timer timer(timeout);
    bool close = false;
    float prevLateralOut = 0; // previous lateral power
    float prevAngularOut = 0; // previous angular power
    const int compState = pros::competition::get_status();

    // main loop
    while ((!timer.isDone() && !lateralLargeExit.getExit() && !lateralSmallExit.getExit()) ||
           (!close && timer.getTimePassed() < 300)) {
        // update position
        const Pose pose = getPose(true, true);

        // update distance travelled
        distTravelled = pose.distance(lastPose);
        lastPose = pose;

        // calculate distance to the target point
        const float distTarget = pose.distance(target);

        // check if the robot is close enough to the target to start settling
        if (distTarget < 7.5 && close == false) {
            close = true;
            maxSpeed = fmax(fabs(prevLateralOut), 60);
        }

        // calculate the carrot point
        Pose carrot = target - Pose(cos(target.theta), sin(target.theta)) * lead * distTarget;
        if (close) carrot = target; // settling behavior

        // calculate error
        const float adjustedRobotTheta = forwards ? pose.theta : pose.theta + M_PI;
        const float angularError =
            close ? angleError(adjustedRobotTheta, target.theta) : angleError(adjustedRobotTheta, pose.angle(carrot));
        const float targetThetaLateral = close ? target.theta : pose.angle(carrot);
        const float lateralError = pose.distance(carrot) * cos(angleError(pose.theta, targetThetaLateral));

        // update exit conditions
        lateralSmallExit.update(lateralError);
        lateralLargeExit.update(lateralError);
        angularSmallExit.update(angularError);
        angularLargeExit.update(angularError);

        // get output from PIDs
        float lateralOut = lateralPID.update(lateralError);
        float angularOut = angularPID.update(angularError);

        // apply restrictions on angular speed
        angularOut = std::clamp(angularOut, -maxSpeed, maxSpeed);
        angularOut = slew(angularOut, prevAngularOut, angularSettings.slew);

        // constrain lateral output by max speed
        lateralOut = std::clamp(lateralOut, -maxSpeed, maxSpeed);
        // constrain lateral output by max accel
        // but not for decelerating, since that would interfere with settling
        if (forwards && lateralOut > prevLateralOut)
            lateralOut = slew(lateralOut, prevLateralOut, lateralSettings.slew);
        if (!forwards && lateralOut < prevLateralOut)
            lateralOut = slew(lateralOut, prevLateralOut, lateralSettings.slew);
        // constrain lateral output by the max speed it can travel at without slipping
        const float radius = 1 / fabs(getCurvature(pose, carrot));
        const float maxSlipSpeed(sqrt(chasePower * radius * 9.8));
        lateralOut = std::clamp(lateralOut, -maxSlipSpeed, maxSlipSpeed);

        // prevent moving in the wrong direction
        if (forwards) lateralOut = std::fmax(lateralOut, 0);
        else lateralOut = std::fmin(lateralOut, 0);

        // update previous output
        prevAngularOut = angularOut;
        prevLateralOut = lateralOut;

        // move the drivetrain
        drivetrain.leftMotors->move(lateralOut + angularOut);
        drivetrain.leftMotors->move(lateralOut - angularOut);

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
 * @brief Move the chassis towards a target point
 *
 * @param x x location
 * @param y y location
 * @param timeout longest time the robot can spend moving
 * @param maxSpeed the maximum speed the robot can move at. 127 by default
 * @param async whether the function should be run asynchronously. true by default
 */
void lemlib::Chassis::moveToPoint(float x, float y, int timeout, bool forwards, float maxSpeed, bool async) {
    // take the mutex
    mutex.take(TIMEOUT_MAX);
    // if the function is async, run it in a new task
    if (async) {
        pros::Task task([&]() { moveToPoint(x, y, timeout, forwards, maxSpeed, false); });
        mutex.give();
        pros::delay(10); // delay to give the task time to start
        return;
    }

    Pose target(x, y); // target pose in standard form
    Pose lastPose = getPose(); // last pose
    float prevLinearPower = 0; // previous linear power
    int compState = pros::competition::get_status();
    int start = pros::millis();
    distTravelled = 0;

    // if (!forwards) target.theta = fmod(target.theta + M_PI, 2 * M_PI); // backwards movement

    // main loop
    while (pros::competition::get_status() == compState && (!linearPID.settled() || pros::millis() - start < 300)) {
        // get current pose
        Pose pose = getPose(true);
        if (!forwards) pose.theta += M_PI;
        pose.theta = M_PI_2 - pose.theta; // convert to standard form

        // settling behaviour
        bool close = pose.distance(target) < 7.5;

        // update completion vars
        distTravelled += pose.distance(lastPose);
        lastPose = pose;

        target.theta = pose.angle(target);
        // calculate error
        float angularError = angleError(pose.theta, target.theta); // angular error
        // linear error
        float linearError = pose.distance(target);
        linearError *= cos(angularError);
        if (!forwards) linearError = -linearError;

        // get PID outputs
        float angularPower = -angularPID.update(radToDeg(angularError), 0);
        float linearPower = linearPID.update(linearError, 0);
        if (!close && forwards && linearPower < 0) linearPower = 0;
        else if (!close && !forwards && linearPower > 0) linearPower = 0;

        // cap acceleration
        if (forwards && !close && linearPower > prevLinearPower)
            linearPower = slew(linearPower, prevLinearPower, linearSettings.slew);
        else if (!forwards && !close && linearPower < prevLinearPower)
            linearPower = slew(linearPower, prevLinearPower, linearSettings.slew);

        prevLinearPower = linearPower;

        // calculate motor powers
        float leftPower = linearPower + angularPower;
        float rightPower = linearPower - angularPower;

        // move the motors
        drivetrain.leftMotors->move(leftPower);
        drivetrain.rightMotors->move(rightPower);

        pros::delay(10); // delay to save resources
    }

    // stop the drivetrain
    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
    // set distTraveled to -1 to indicate that the function has finished
    distTravelled = -1;
    // give the mutex back
    mutex.give();
}
