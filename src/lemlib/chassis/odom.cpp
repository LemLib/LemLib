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
#include "lemlib/util.hpp"
#include "lemlib/chassis/odom.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/trackingWheel.hpp"

using namespace lemlib;

/**
 * Odometry update function
 *
 */
void Odometry::update() {
    // TODO: add particle filter
    // get the current sensor values
    float vertical1Raw = 0;
    float vertical2Raw = 0;
    float horizontal1Raw = 0;
    float horizontal2Raw = 0;
    float imuRaw = 0;
    if (chassis->odomSensors.vertical1 != nullptr) vertical1Raw = chassis->odomSensors.vertical1->getDistanceTraveled();
    if (chassis->odomSensors.vertical2 != nullptr) vertical2Raw = chassis->odomSensors.vertical2->getDistanceTraveled();
    if (chassis->odomSensors.horizontal1 != nullptr)
        horizontal1Raw = chassis->odomSensors.horizontal1->getDistanceTraveled();
    if (chassis->odomSensors.horizontal2 != nullptr)
        horizontal2Raw = chassis->odomSensors.horizontal2->getDistanceTraveled();
    if (chassis->odomSensors.imu != nullptr) imuRaw = degToRad(chassis->odomSensors.imu->get_rotation());

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
    // 1. Inertial Sensor
    // 2. Dual vertical tracking wheels
    // 3. Dual horizontal tracking wheels
    // 4. Single vertical tracking wheel w/ drivetrain
    // 5. Single horizontal tracking wheel w/ drivetrain
    // 6. Drivetrain

    // 1. Horizontal tracking wheels
    // 2. Vertical tracking wheels
    // 3. Inertial Sensor
    // 4. Drivetrain
    float heading = pose.theta;
    // calculate the heading using the horizontal tracking wheels
    if (chassis->odomSensors.horizontal1 != nullptr && chassis->odomSensors.horizontal2 != nullptr)
        heading += (deltaHorizontal1 - deltaHorizontal2) /
                   (chassis->odomSensors.horizontal1->getOffset() - chassis->odomSensors.horizontal2->getOffset());
    // else, if both vertical tracking wheels aren't substituted by the drivetrain, use the vertical tracking wheels
    else if (!chassis->odomSensors.vertical1->getType() && !chassis->odomSensors.vertical2->getType())
        heading += (deltaVertical1 - deltaVertical2) /
                   (chassis->odomSensors.vertical1->getOffset() - chassis->odomSensors.vertical2->getOffset());
    // else, if the inertial sensor exists, use it
    else if (chassis->odomSensors.imu != nullptr) heading += deltaImu;
    // else, use the the substituted tracking wheels
    else
        heading += (deltaVertical1 - deltaVertical2) /
                   (chassis->odomSensors.vertical1->getOffset() - chassis->odomSensors.vertical2->getOffset());
    float deltaHeading = heading - pose.theta;
    float avgHeading = pose.theta + deltaHeading / 2;

    // choose tracking wheels to use
    // Prioritize non-powered tracking wheels
    lemlib::TrackingWheel* verticalWheel = nullptr;
    lemlib::TrackingWheel* horizontalWheel = nullptr;
    if (!chassis->odomSensors.vertical1->getType()) verticalWheel = chassis->odomSensors.vertical1;
    else if (!chassis->odomSensors.vertical2->getType()) verticalWheel = chassis->odomSensors.vertical2;
    else verticalWheel = chassis->odomSensors.vertical1;
    if (chassis->odomSensors.horizontal1 != nullptr) horizontalWheel = chassis->odomSensors.horizontal1;
    else if (chassis->odomSensors.horizontal2 != nullptr) horizontalWheel = chassis->odomSensors.horizontal2;
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
