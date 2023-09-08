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

/**
 * Chassis constructor
 *
 * The chassis constructor simply takes in structs for chassis configuration and stores them
 * in private members of the Chassis class. The structs are defined in chassis.hpp
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
 * Chassis calibration
 *
 * The chassis calibration function calibrates sensors and initializes the odom.
 *
 * The function checks the odomSensors struct to see what sensors are available.
 * It knows what sensors are not available by checking if the pointers are null.
 *
 * If a left or right vertical tracking wheel is not available, it simply substitutes
 * one side of the drivetrain for that tracking wheel.
 *
 * It has logic to continuously calibrate the imu until it calibrates successfully
 * and vibrates the controller on every failure to alert the user.
 *
 * If the chassis has been calibrated successfully, it vibrates the controller once
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
 * Set the chassis's position and heading
 *
 * This function is a wrapper for the setPose function in odom.hpp
 */
void lemlib::Chassis::setPose(float x, float y, float theta, bool radians) {
    lemlib::setPose(lemlib::Pose(x, y, theta), radians);
}

/**
 * Sets the chassis's position and heading
 *
 * This function is a wrapper for the setPose function in odom.hpp
 */
void lemlib::Chassis::setPose(Pose pose, bool radians) { lemlib::setPose(pose, radians); }

/**
 * Gets the chassis's position and heading and returns it as a Pose
 *
 * This function is a wrapper for the getPose function in odom.hpp
 */
lemlib::Pose lemlib::Chassis::getPose(bool radians) { return lemlib::getPose(radians); }

/**
 * Turns the robot to face a point
 *
 * Instead of using a defined heading, this function uses a point to determine which angle
 * the robot should turn to. Using a heading assumes the robot's starting position is constant
 * which of course its not
 *
 * It uses the generic LemLib PID class to calculate the power needed to the point.
 * It also uses it for exit conditions.
 *
 * If the competition state changes, the function will stop. This prevents the code from
 * controlling the robot when its not supposed to, but still allows for the function
 * to be called in driver control for testing.
 *
 * Thanks to the angleError function, the robot will always turn the fastest way.
 *
 * Its logging id is "angularPID" if you want to see its logs.
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
 * Moves the robot to a target position and heading
 *
 * This function uses the boomerang controller to move the robot. It generates a "carrot point"
 * which the robot will follow. The carrot point is a point that is certain distance away from
 * the target point, specifically the lead constant times the distance from the robot to the target.
 * The carrot point is also a specific angle from the target point, determined by the target heading.
 *
 * The algorithm uses 2 generic LemLib PID controllers, one for linear movement and one for angular movement.
 * The linear controller is also used for exit conditions.
 *
 * Since the robot is moving while turning, a few optimizations are made to improve performance.
 * The robot always prioritizes turning over moving, decreasing the chances the robot will overshoot
 * turns. In addition, a calculation is performed to determine the maximum speed the robot can travel
 * at while performing a turn of a specific radius. This is done using the formula v = sqrt( u * r * g )
 * where u is a constant, tuned by the user. The robot will then cap its speed to v.
 *
 * The reason this is done is to prevent overshooting turns, which wastes time and causes unpredictable
 * movement. Robots without traction wheels are especially prone to overshooting turns, but this algorithm
 * prevents that if tuned correctly.
 */
void lemlib::Chassis::moveTo(float x, float y, float theta, bool forwards, int timeout, float chasePower, float lead,
                             float maxSpeed, bool log) {
    Pose target(x, y, M_PI_2 - degToRad(theta)); // target pose in standard form
    FAPID linearPID = FAPID(0, 0, lateralSettings.kP, 0, lateralSettings.kD, "linearPID");
    linearPID.setExit(lateralSettings.largeError, lateralSettings.smallError, lateralSettings.smallErrorTimeout,
                      lateralSettings.smallErrorTimeout, timeout); // exit conditions
    FAPID angularPID = FAPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    int compState = pros::competition::get_status();
    int start = pros::millis();

    if (!forwards) target.theta = fmod(target.theta + M_PI, 2 * M_PI); // backwards movement

    bool close = false; // used for settling
    if (chasePower == 0) chasePower = drivetrain.chasePower; // use global chase power if chase power is 0

    // main loop
    while (pros::competition::get_status() == compState && (!linearPID.settled() || pros::millis() - start < 300)) {
        // get current pose
        Pose pose = getPose(true);
        if (!forwards) pose.theta += M_PI;
        pose.theta = M_PI_2 - pose.theta; // convert to standard form

        // check if the robot is close enough to the target to start settling
        if (pose.distance(target) < 7.5) close = true;

        // calculate the carrot point
        Pose carrot = target - (Pose(cos(target.theta), sin(target.theta)) * lead * pose.distance(target));
        if (close) carrot = target; // settling behavior

        // calculate error
        float angularError = angleError(pose.angle(carrot), pose.theta, true); // angular error
        float linearError = pose.distance(carrot) * cos(angularError); // linear error
        if (close) angularError = angleError(target.theta, pose.theta, true); // settling behavior
        if (!forwards) linearError = -linearError;

        // get PID outputs
        float angularPower = -angularPID.update(radToDeg(angularError), 0, log);
        float linearPower = linearPID.update(linearError, 0, log);

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
}
