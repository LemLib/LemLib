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
#include "lemlib/chassis/trackingWheel.hpp"
#include "pros/rtos.h"

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
 * @param kP proportional constant for the chassis controller
 * @param kD derivative constant for the chassis controller
 * @param smallError the error at which the chassis controller will switch to a slower control loop
 * @param smallErrorTimeout the time the chassis controller will wait before switching to a slower control loop
 * @param largeError the error at which the chassis controller will switch to a faster control loop
 * @param largeErrorTimeout the time the chassis controller will wait before switching to a faster control loop
 * @param slew the maximum acceleration of the chassis controller
 */
lemlib::ControllerSettings::ControllerSettings(float kP, float kD, float smallError, float smallErrorTimeout,
                                               float largeError, float largeErrorTimeout, float slew)
    : kP(kP),
      kD(kD),
      smallError(smallError),
      smallErrorTimeout(smallErrorTimeout),
      largeError(largeError),
      largeErrorTimeout(largeErrorTimeout),
      slew(slew) {}

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
 * @param linearSettings settings for the linear controller
 * @param angularSettings settings for the angular controller
 * @param sensors sensors to be used for odometry
 * @param driveCurve drive curve to be used. defaults to `defaultDriveCurve`
 */
lemlib::Chassis::Chassis(Drivetrain drivetrain, ControllerSettings linearSettings, ControllerSettings angularSettings,
                         OdomSensors sensors, DriveCurveFunction_t driveCurve)
    : drivetrain(drivetrain),
      linearSettings(linearSettings),
      angularSettings(angularSettings),
      sensors(sensors),
      driveCurve(driveCurve) {}

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
lemlib::Pose lemlib::Chassis::getPose(bool radians) { return lemlib::getPose(radians); }

/**
 * @brief Get the speed of the robot
 *
 * @param radians true for theta in radians, false for degrees. False by default
 * @return lemlib::Pose
 */
lemlib::Pose lemlib::Chassis::getSpeed(bool radians) { return lemlib::getSpeed(radians); }

/**
 * @brief Get the local speed of the robot
 *
 * @param radians true for theta in radians, false for degrees. False by default
 * @return lemlib::Pose
 */
lemlib::Pose lemlib::Chassis::getLocalSpeed(bool radians) { return lemlib::getLocalSpeed(radians); }

/**
 * @brief Estimate the pose of the robot after a certain amount of time
 *
 * @param time time in seconds
 * @param radians False for degrees, true for radians. False by default
 * @return lemlib::Pose
 */
lemlib::Pose lemlib::Chassis::estimatePose(float time, bool radians) { return lemlib::estimatePose(time, radians); }

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
        mutex.give();
        pros::delay(10); // delay to give the task time to start
        return;
    }

    Pose target(x, y, M_PI_2 - degToRad(theta)); // target pose in standard form
    Pose lastPose = getPose(); // last pose
    FAPID linearPID = FAPID(0, 0, linearSettings.kP, 0, linearSettings.kD, "linearPID");
    FAPID angularPID = FAPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    linearPID.setExit(linearSettings.largeError, linearSettings.smallError, linearSettings.smallErrorTimeout,
                      linearSettings.smallErrorTimeout, timeout); // exit conditions
    float prevLinearPower = 0; // previous linear power
    int compState = pros::competition::get_status();
    int start = pros::millis();
    distTravelled = 0;

    if (!forwards) target.theta = fmod(target.theta + M_PI, 2 * M_PI); // backwards movement

    bool close = false; // used for settling
    if (chasePower == 0) chasePower = drivetrain.chasePower; // use global chase power if chase power is 0

    // main loop
    while (pros::competition::get_status() == compState && (!linearPID.settled() || pros::millis() - start < 300)) {
        // get current pose
        Pose pose = getPose(true);
        if (!forwards) pose.theta += M_PI;
        pose.theta = M_PI_2 - pose.theta; // convert to standard form

        // update completion vars
        distTravelled += pose.distance(lastPose);
        lastPose = pose;

        // check if the robot is close enough to the target to start settling
        if (pose.distance(target) < 7.5 && close == false) {
            close = true;
            maxSpeed = fmax(fabs(prevLinearPower), 30);
        }

        // calculate the carrot point
        Pose carrot = target - (Pose(cos(target.theta), sin(target.theta)) * lead * pose.distance(target));
        if (close) carrot = target; // settling behavior

        // calculate error
        float angularError = angleError(pose.angle(carrot), pose.theta, true); // angular error
        // linear error
        float linearError = pose.distance(carrot);
        if (close) linearError *= cos(angularError);
        else linearError *= std::fmax(cos(angularError), 0);
        if (close) angularError = angleError(target.theta, pose.theta, true); // settling behavior
        if (!forwards) linearError = -linearError;

        // get PID outputs
        float angularPower = -angularPID.update(radToDeg(angularError), 0);
        float linearPower = linearPID.update(linearError, 0);

        // calculate radius of turn
        float curvature = fabs(getCurvature(pose, carrot));
        if (curvature == 0) curvature = -1;
        float radius = 1 / curvature;

        // calculate the maximum speed at which the robot can turn
        // using the formula v = sqrt( u * r * g )
        if (radius != -1) {
            float maxTurnSpeed = sqrt(chasePower * radius * 9.8);
            // the new linear power is the minimum of the linear power and the max turn speed
            if (linearPower > maxTurnSpeed && !close) linearPower = maxTurnSpeed;
            else if (linearPower < -maxTurnSpeed && !close) linearPower = -maxTurnSpeed;
        }

        // prioritize turning over moving
        float overturn = fabs(angularPower) + fabs(linearPower) - maxSpeed;
        if (overturn > 0) linearPower -= linearPower > 0 ? overturn : -overturn;

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

    Pose pose(0, 0);
    Pose lastPose = getPose();
    float prevLateralPower = 0;
    float prevAngularPower = 0;
    bool close = false;
    int start = pros::millis();
    std::uint8_t compState = pros::competition::get_status();
    distTravelled = 0;

    // create a new PID controller
    FAPID lateralPID(0, 0, linearSettings.kP, 0, linearSettings.kD, "lateralPID");
    FAPID angularPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    lateralPID.setExit(linearSettings.largeError, linearSettings.smallError, linearSettings.largeErrorTimeout,
                       linearSettings.smallErrorTimeout, timeout);

    // main loop
    while (pros::competition::get_status() == compState && (!lateralPID.settled() || pros::millis() - start < 300)) {
        Pose target(x, y);
        // get the current position
        Pose pose = getPose(true);

        // update completion vars
        distTravelled += pose.distance(lastPose);
        lastPose = pose;

        // update error
        float targetTheta = M_PI_2 - pose.angle(target);
        float hypot = pose.distance(target);
        float angularError =
            (forwards) ? angleError(pose.theta, targetTheta, true) : angleError(pose.theta, targetTheta + M_PI, true);
        float linearError = hypot;
        if (close) linearError *= cos(angleError(pose.theta, targetTheta));
        else linearError *= std::fmax(angleError(pose.theta, targetTheta), 0);

        // calculate speed
        float lateralPower = lateralPID.update(linearError, 0);
        float angularPower = -angularPID.update(radToDeg(angularError), 0);

        // if the robot is close to the target
        if (pose.distance(target) < 7.5) {
            close = true;
            maxSpeed = (std::fabs(prevLateralPower) < 30) ? 30 : std::fabs(prevLateralPower);
        }

        // limit acceleration
        if (!close) lateralPower = lemlib::slew(lateralPower, prevLateralPower, linearSettings.slew);
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

    // set distTraveled to -1 to indicate that the function has finished
    distTravelled = -1;
    // give the mutex back
    mutex.give();
}
