/**
 * @file src/lemlib/chassis/odom.cpp
 * @author LemLib Team
 * @brief Odometry source file. Contains odometry functions and global variables
 * @version 0.4.5
 * @date 2023-01-27
 *
 * @copyright Copyright (c) 2023
 *
 */

// The implementation below is mostly based off of
// the document written by 5225A (Pilons)
// Here is a link to the original document
// http://thepilons.ca/wp-content/uploads/2018/10/Tracking.pdf

#include <math.h>
#include <cerrno>
#include "pros/misc.h"
#include "lemlib/util.hpp"
#include "lemlib/chassis/odom.hpp"
#include "lemlib/devices/trackingWheel.hpp"

using namespace lemlib;

/**
 * Calibrate odometry sensors
 *
 * This function should be called before Odometry::update() runs for the first time.
 *
 * First, we attempt to calibrate the imu. IMU calibration failure is common, so we
 * attempt to calibrate it up to 5 times if it fails. If it still fails after this, we
 * rumble the controller to let the user know that the imu failed to calibrate. If it
 * does fail to calibrate, we will use other sensors to calculate the heading of the robot.
 *
 * Calibrating the tracking wheels is simpler. If we detect there are less than 2 vertical
 * tracking wheels, we will simply substitute one side of the drivetrain, and then we
 * calibrate them. For horizontal wheels, we just calibrate them if they exist.
 */
void Odometry::calibrate(bool calibrateIMU) {
    // calibrate the imu if it exists
    if (sensors.imu != nullptr && calibrateIMU) {
        int attempt = 1;
        // calibrate inertial, and if calibration fails, then repeat 5 times or until successful
        while (sensors.imu->reset(true) != 1 && (errno == PROS_ERR || errno == ENODEV || errno == ENXIO) &&
               attempt < 5) {
            attempt++;
            pros::delay(50);
        }
        if (attempt == 5) {
            // disable IMU
            sensors.imu = nullptr;
            // rumble controller if imu calibration fails
            pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "---");
        }
    }
    // substitute tracking wheels with a side of the drivetrain if needed
    if (sensors.vertical1 == nullptr)
        sensors.vertical1 = new TrackingWheel(std::shared_ptr<pros::MotorGroup>(drive.leftMotors), drive.wheelDiameter,
                                              -(drive.trackWidth / 2), drive.rpm);
    if (sensors.vertical2 == nullptr)
        sensors.vertical2 = new TrackingWheel(std::shared_ptr<pros::MotorGroup>(drive.rightMotors), drive.wheelDiameter,
                                              drive.trackWidth / 2, drive.rpm);
    // calibrate the tracking wheels
    sensors.vertical1->reset();
    sensors.vertical2->reset();
    if (sensors.horizontal1 != nullptr) sensors.horizontal1->reset();
    if (sensors.horizontal2 != nullptr) sensors.horizontal2->reset();
}

/**
 * Gets the pose
 */
Pose Odometry::getPose() { return pose; }

/**
 * Sets the pose
 */
void Odometry::setPose(Pose pose) { this->pose = pose; }

/**
 * Odometry update function
 *
 * Odometry is what we call position tracking. We use odometry so we can
 * use the position of the robot to correct for errors during autonomous.
 *
 * This function checks which sensors are configured and uses them to calculate
 * the position of the robot. This ability means that almost any odometry configuration
 * will work with this library, all the user has to do is pass the sensors.
 * It will also use the best combination of sensors available, so if the user can
 * just pass everything and know the best sensors will be used.
 *
 * In future, an extended Kalman filter will be used so all the sensors can be used.
 */
void Odometry::update() {
    // get the current sensor values
    float vertical1Raw = 0;
    float vertical2Raw = 0;
    float horizontal1Raw = 0;
    float horizontal2Raw = 0;
    float imuRaw = 0;
    if (sensors.vertical1 != nullptr) vertical1Raw = sensors.vertical1->getDistance();
    if (sensors.vertical2 != nullptr) vertical2Raw = sensors.vertical2->getDistance();
    if (sensors.horizontal1 != nullptr) horizontal1Raw = sensors.horizontal1->getDistance();
    if (sensors.horizontal2 != nullptr) horizontal2Raw = sensors.horizontal2->getDistance();
    if (sensors.imu != nullptr) imuRaw = degToRad(sensors.imu->get_rotation());

    // calculate the change in sensor values
    float deltaVertical1 = vertical1Raw - prevVertical1;
    float deltaVertical2 = vertical2Raw - prevVertical2;
    float deltaHorizontal1 = horizontal1Raw - prevHorizontal1;
    float deltaHorizontal2 = horizontal2Raw - prevHorizontal2;
    float deltaImu = imuRaw - prevImu;

    // update the previous sensor values
    prevVertical1 = vertical1Raw;
    prevVertical2 = vertical2Raw;
    prevHorizontal1 = horizontal1Raw;
    prevHorizontal2 = horizontal2Raw;
    prevImu = imuRaw;

    // calculate the heading of the robot
    // Priority:
    // 1. IMU
    // 2. Horizontal tracking wheels
    // 3. Vertical tracking wheels
    float heading = pose.theta;
    // calculate heading with inertial sensor if it exists
    if (sensors.imu != nullptr) heading += deltaImu;
    // else, use horizontal tracking wheels if they both exist
    else if (sensors.horizontal1 != nullptr && sensors.horizontal2 != nullptr)
        heading += (deltaHorizontal1 - deltaHorizontal2) /
                   (sensors.horizontal1->getOffset() - sensors.horizontal2->getOffset());
    else
        heading +=
            (deltaVertical1 - deltaVertical2) / (sensors.vertical1->getOffset() - sensors.vertical2->getOffset());
    float deltaHeading = heading - pose.theta;
    float avgHeading = pose.theta + deltaHeading / 2;

    // choose tracking wheels to use
    TrackingWheel* verticalWheel = sensors.vertical1;
    TrackingWheel* horizontalWheel = nullptr;
    // horizontal tracking wheels
    if (sensors.horizontal1 != nullptr) horizontalWheel = sensors.horizontal1;
    if (sensors.horizontal2 != nullptr) horizontalWheel = sensors.horizontal2;
    // get raw values
    float rawVertical = 0;
    float rawHorizontal = 0;
    rawVertical = verticalWheel->getDistance();
    if (horizontalWheel != nullptr) rawHorizontal = horizontalWheel->getDistance();
    // get offsets
    float horizontalOffset = 0;
    float verticalOffset = 0;
    verticalOffset = verticalWheel->getOffset();
    if (horizontalWheel != nullptr) horizontalOffset = horizontalWheel->getOffset();

    // calculate change in x and y
    float deltaX = 0;
    float deltaY = 0;
    deltaY = rawVertical - prevVertical;
    if (horizontalWheel != nullptr) deltaX = rawHorizontal - prevHorizontal;
    prevVertical = rawVertical;
    prevHorizontal = rawHorizontal;

    // calculate local x and y
    float localX = 0;
    float localY = 0;
    if (deltaHeading == 0) { // prevent divide by 0
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
}
