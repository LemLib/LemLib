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
#include "lemlib/logger.hpp"
#include "lemlib/pose.hpp"
#include "pros/rtos.hpp"
#include "lemlib/util.hpp"
#include "lemlib/chassis/odom.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/trackingWheel.hpp"

// tracking thread
pros::Task* trackingTask = nullptr;

// global variables
lemlib::OdomSensors odomSensors; // the sensors to be used for odometry
// lemlib::Drivetrain_t drive; // the drivetrain to be used for odometry
lemlib::Pose odomPose(0, 0, 0); // the pose of the robot

float prevVertical = 0;
float prevVertical1 = 0;
float prevVertical2 = 0;
float prevHorizontal = 0;
float prevHorizontal1 = 0;
float prevHorizontal2 = 0;
float prevImu = 0;

float prevGpsX = 0;
float prevGpsY = 0;
float prevGpsRotation = 0;

/**
 * @brief Set the sensors to be used for odometry
 *
 * @param sensors the sensors to be used
 */
void lemlib::setSensors(lemlib::OdomSensors sensors) { odomSensors = sensors; }

/**
 * @brief Get the pose of the robot
 *
 * @param radians true for theta in radians, false for degrees. False by default
 * @return Pose
 */
lemlib::Pose lemlib::getPose(bool radians) {
    return lemlib::Pose(odomPose.x, odomPose.y, radians ? odomPose.theta : degToRad(odomPose.theta));
}

/**
 * @brief Set the Pose of the robot
 *
 * @param pose the new pose
 * @param radians true if theta is in radians, false if in degrees. False by default
 */
void lemlib::setPose(lemlib::Pose pose, bool radians) {
    odomPose = lemlib::Pose(pose.x, pose.y, radians ? pose.theta : degToRad(pose.theta));
}

static void updateGPS() {
    pros::c::gps_status_s_t status = odomSensors.gps->get_status();

    float gpsRawX = status.x;
    float gpsRawY = status.y;
    float gpsRawRotation = lemlib::degToRad(odomSensors.gps->get_rotation());

    // calculate the change in sensor values
    float deltaGpsX = gpsRawX - prevGpsX;
    float deltaGpsY = gpsRawY - prevGpsY;
    float deltaGpsRotation = gpsRawRotation - prevGpsRotation;

    // update the previous sensor values
    prevGpsX = gpsRawX;
    prevGpsY = gpsRawY;
    prevGpsRotation = gpsRawRotation;

    // update the pose
    odomPose.x += deltaGpsX;
    odomPose.y += deltaGpsY;
    odomPose.theta += deltaGpsRotation;
}

static void updateWheels() {
    float vertical1Raw = 0;
    float vertical2Raw = 0;
    float horizontal1Raw = 0;
    float horizontal2Raw = 0;
    float imuRaw = 0;
    if (odomSensors.vertical1 != nullptr) vertical1Raw = odomSensors.vertical1->getDistanceTraveled();
    if (odomSensors.vertical2 != nullptr) vertical2Raw = odomSensors.vertical2->getDistanceTraveled();
    if (odomSensors.horizontal1 != nullptr) horizontal1Raw = odomSensors.horizontal1->getDistanceTraveled();
    if (odomSensors.horizontal2 != nullptr) horizontal2Raw = odomSensors.horizontal2->getDistanceTraveled();
    if (odomSensors.imu != nullptr) imuRaw = lemlib::degToRad(odomSensors.imu->get_rotation());

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
    // 1. Horizontal tracking wheels
    // 2. Vertical tracking wheels
    // 3. Inertial Sensor
    // 4. Drivetrain
    float heading = odomPose.theta;
    // calculate the heading using the horizontal tracking wheels
    if (odomSensors.horizontal1 != nullptr && odomSensors.horizontal2 != nullptr)
        heading += (deltaHorizontal1 - deltaHorizontal2) /
                   (odomSensors.horizontal1->getOffset() - odomSensors.horizontal2->getOffset());
    // else, if both vertical tracking wheels aren't substituted by the drivetrain, use the vertical tracking wheels
    else if (!odomSensors.vertical1->getType() && !odomSensors.vertical2->getType())
        heading += (deltaVertical1 - deltaVertical2) /
                   (odomSensors.vertical1->getOffset() - odomSensors.vertical2->getOffset());
    // else, if the inertial sensor exists, use it
    else if (odomSensors.imu != nullptr) heading += deltaImu;
    // else, use the the substituted tracking wheels
    else
        heading += (deltaVertical1 - deltaVertical2) /
                   (odomSensors.vertical1->getOffset() - odomSensors.vertical2->getOffset());
    float deltaHeading = heading - odomPose.theta;
    float avgHeading = odomPose.theta + deltaHeading / 2;

    // choose tracking wheels to use
    // Prioritize non-powered tracking wheels
    lemlib::TrackingWheel* verticalWheel = nullptr;
    lemlib::TrackingWheel* horizontalWheel = nullptr;
    if (!odomSensors.vertical1->getType()) verticalWheel = odomSensors.vertical1;
    else if (!odomSensors.vertical2->getType()) verticalWheel = odomSensors.vertical2;
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
    if (deltaHeading == 0) { // prevent divide by 0
        localX = deltaX;
        localY = deltaY;
    } else {
        localX = 2 * sin(deltaHeading / 2) * (deltaX / deltaHeading + horizontalOffset);
        localY = 2 * sin(deltaHeading / 2) * (deltaY / deltaHeading + verticalOffset);
    }

    // calculate global x and y
    odomPose.x += localY * sin(avgHeading);
    odomPose.y += localY * cos(avgHeading);
    odomPose.x += localX * -cos(avgHeading);
    odomPose.y += localX * sin(avgHeading);
    odomPose.theta = heading;
}

/**
 * @brief Update the pose of the robot
 *
 */
void lemlib::update() {
    if (odomSensors.gps == nullptr) {
        updateWheels();
    } else {
        updateGPS();
    }
}

/**
 * @brief Initialize the odometry system
 *
 */
void lemlib::init() {
    if (trackingTask == nullptr) {
        trackingTask = new pros::Task {[=] {
            while (true) {
                update();
                pros::delay(10);
            }
        }};
    }
}
