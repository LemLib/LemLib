/**
 * @file lemlib/chassis/odom.cpp
 * @author Liam Teale
 * @brief Odometry source file. Contains odometry functions and global variables
 * @version 0.1
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
#include "pros/rtos.hpp"
#include "lemlib/util.hpp"
#include "lemlib/chassis/odom.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/trackingWheel.hpp"


// tracking thread
pros::Task *trackingTask = nullptr;


// global variables
lemlib::OdomSensors_t odomSensors; // the sensors to be used for odometry
lemlib::Pose odomPose(0, 0, 0); // the pose of the robot

float prevVertical = 0;
float prevVertical1 = 0;
float prevVertical2 = 0;
float prevHorizontal = 0;
float prevHorizontal1 = 0;
float prevHorizontal2 = 0;
float prevImu = 0;

/**
 * @brief Set the sensors to be used for odometry
 * 
 * @param sensors the sensors to be used
 */
void lemlib::setSensors(lemlib::OdomSensors_t sensors)
{
    odomSensors = sensors;
}


/**
 * @brief Get the pose of the robot
 * 
 * @param radians true for theta in radians, false for degrees. False by default
 * @return Pose 
 */
lemlib::Pose lemlib::getPose(bool radians)
{
    if (radians)
    {
        return odomPose;
    }
    else
    {
        return lemlib::Pose(odomPose.x, odomPose.y, radToDeg(odomPose.theta));
    }
}


/**
 * @brief Set the Pose of the robot
 * 
 * @param pose the new pose
 * @param radians true if theta is in radians, false if in degrees. False by default
 */
void lemlib::setPose(lemlib::Pose pose, bool radians)
{
    if (radians)
    {
        odomPose = pose;
    }
    else
    {
        odomPose = lemlib::Pose(pose.x, pose.y, degToRad(pose.theta));
    }
}


/**
 * @brief Update the pose of the robot
 * 
 */
void lemlib::update() 
{
    // TODO: add particle filter
    // get the current sensor values
    float vertical1Raw = 0;
    float vertical2Raw = 0;
    float horizontal1Raw = 0;
    float horizontal2Raw = 0;
    float imuRaw = 0;
    if (odomSensors.vertical1 != nullptr) vertical1Raw = odomSensors.vertical1->getDistanceTraveled();
    if (odomSensors.vertical2 != nullptr) vertical2Raw = odomSensors.vertical2->getDistanceTraveled();
    if (odomSensors.horizontal1 != nullptr) horizontal1Raw = odomSensors.horizontal1->getDistanceTraveled();
    if (odomSensors.horizontal2 != nullptr) horizontal2Raw = odomSensors.horizontal2->getDistanceTraveled();
    if (odomSensors.imu != nullptr) imuRaw = degToRad(odomSensors.imu->get_rotation());

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
    float heading = odomPose.theta;
    if (odomSensors.vertical1 != nullptr && odomSensors.vertical2 != nullptr) heading += (deltaVertical1 - deltaVertical2) / (odomSensors.vertical1->getOffset() - odomSensors.vertical2->getOffset());
    else if (odomSensors.horizontal1 != nullptr && odomSensors.horizontal2 != nullptr) heading += (deltaHorizontal1 - deltaHorizontal2) / (odomSensors.horizontal1->getOffset() - odomSensors.horizontal2->getOffset());
    else if (odomSensors.imu != nullptr) heading += deltaImu;
    float deltaHeading = heading - odomPose.theta;
    float avgHeading = odomPose.theta + deltaHeading / 2;

    // choose tracking wheels to use
    lemlib::TrackingWheel *verticalWheel = nullptr;
    lemlib::TrackingWheel *horizontalWheel = nullptr;
    if (odomSensors.vertical1 != nullptr) verticalWheel = odomSensors.vertical1;
    else if (odomSensors.vertical2 != nullptr) verticalWheel = odomSensors.vertical2;
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
        localX = 2 * sin(deltaHeading/2) * (deltaX/deltaHeading + horizontalOffset);
        localY = 2 * sin(deltaHeading/2) * (deltaY/deltaHeading + verticalOffset);
    }

    // calculate global x and y
    odomPose.x += localY * sin(avgHeading);
    odomPose.y += localY * cos(avgHeading);
    odomPose.x += localX * -cos(avgHeading);
    odomPose.y += localX * sin(avgHeading);
    odomPose.theta = heading;
}


/**
 * @brief Initialize the odometry system
 * 
 */
void lemlib::init()
{
    if (trackingTask == nullptr) {
        trackingTask = new pros::Task{[=] {
            while (true) {
                update();
                pros::delay(10);
            }
        }};
    }
}
