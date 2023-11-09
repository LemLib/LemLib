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
#include "main.h"
#include "pros/motors.hpp"
#include "pros/misc.hpp"
#include "lemlib/util.hpp"
#include "lemlib/pid.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/odom.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "pros/rtos.h"

/**
 * @brief Construct a new Chassis
 *
 * @param drivetrain drivetrain to be used for the chassis
 * @param lateralSettings settings for the lateral controller
 * @param angularSettings settings for the angular controller
 * @param sensors sensors to be used for odometry
 * @param driveCurve drive curve to be used. defaults to `defaultDriveCurve`
 */
lemlib::Chassis::Chassis(Drivetrain_t drivetrain, ChassisController_t lateralSettings,
                         ChassisController_t angularSettings, OdomSensors_t sensors, DriveCurveFunction_t driveCurve) {
    this->drivetrain = drivetrain;
    this->lateralSettings = lateralSettings;
    this->angularSettings = angularSettings;
    this->odomSensors = sensors;
    this->driveCurve = driveCurve;
}

/**
 * @brief Calibrate the chassis sensors
 *
 * @param calibrateIMU whether the IMU should be calibrated. true by default
 */
void lemlib::Chassis::calibrate(bool calibrateIMU) {
    // calibrate the IMU if it exists and the user doesn't specify otherwise
    if (odomSensors.imu != nullptr && calibrateIMU) {
        int attempt = 1;
        // calibrate inertial, and if calibration fails, then repeat 5 times or until successful
        while (odomSensors.imu->reset(true) != 1 && (errno == PROS_ERR || errno == ENODEV || errno == ENXIO) &&
               attempt < 5) {
            pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "---");
            pros::delay(10);
            attempt++;
        }
        if (attempt == 5) odomSensors.imu = nullptr;
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
void lemlib::Chassis::moveTo(float x, float y, float theta, int timeout, bool forwards, float chasePower, float lead,
                             float maxSpeed, bool async) {
    // take the mutex
    mutex.take(TIMEOUT_MAX);
    // if the function is async, run it in a new task
    if (async) {
        pros::Task task([&]() { moveTo(x, y, theta, timeout, forwards, chasePower, lead, maxSpeed, false); });
        mutex.give();
        pros::delay(10); // delay to give the task time to start
        return;
    }

    Pose target(x, y, M_PI_2 - degToRad(theta)); // target pose in standard form
    Pose lastPose = getPose(); // last pose
    FAPID linearPID = FAPID(0, 0, lateralSettings.kP, 0, lateralSettings.kD, "linearPID");
    FAPID angularPID = FAPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    linearPID.setExit(lateralSettings.largeError, lateralSettings.smallError, lateralSettings.smallErrorTimeout,
                      lateralSettings.smallErrorTimeout, timeout); // exit conditions
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
        float linearError = pose.distance(carrot) * cos(angularError); // linear error
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
