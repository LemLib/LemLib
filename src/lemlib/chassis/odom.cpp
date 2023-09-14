// The implementation below is mostly based off of
// the document written by 5225A (Pilons)
// Here is a link to the original document
// http://thepilons.ca/wp-content/uploads/2018/10/Tracking.pdf

#include <math.h>
#include "lemlib/util.hpp"
#include "lemlib/chassis/odom.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/trackingWheel.hpp"

<<<<<<< HEAD
// tracking thread
pros::Task* trackingTask = nullptr;

// global variables
lemlib::OdomSensors odomSensors(nullptr, nullptr, nullptr, nullptr, nullptr); // the sensors to be used for odometry
lemlib::Drivetrain drive(nullptr, nullptr, 0, 0, 0, 0); // the drivetrain to be used for odometry
lemlib::Pose odomPose(0, 0, 0); // the pose of the robot
lemlib::Pose odomSpeed(0, 0, 0); // the speed of the robot
lemlib::Pose odomLocalSpeed(0, 0, 0); // the local speed of the robot

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
 * @param drivetrain drivetrain to be used
 */
void lemlib::setSensors(lemlib::OdomSensors sensors, lemlib::Drivetrain drivetrain) {
    odomSensors = sensors;
    drive = drivetrain;
}

/**
 * @brief Get the pose of the robot
 *
 * @param radians true for theta in radians, false for degrees. False by default
 * @return Pose
 */
lemlib::Pose lemlib::getPose(bool radians) {
    if (radians) return odomPose;
    else return lemlib::Pose(odomPose.x, odomPose.y, radToDeg(odomPose.theta));
}

/**
 * @brief Set the Pose of the robot
 *
 * @param pose the new pose
 * @param radians true if theta is in radians, false if in degrees. False by default
 */
void lemlib::setPose(lemlib::Pose pose, bool radians) {
    if (radians) odomPose = pose;
    else odomPose = lemlib::Pose(pose.x, pose.y, degToRad(pose.theta));
}

/**
 * @brief Get the speed of the robot
 *
 * @param radians true for theta in radians, false for degrees. False by default
 * @return lemlib::Pose
 */
lemlib::Pose lemlib::getSpeed(bool radians) {
    if (radians) return odomSpeed;
    else return lemlib::Pose(odomSpeed.x, odomSpeed.y, radToDeg(odomSpeed.theta));
}

/**
 * @brief Get the local speed of the robot
 *
 * @param radians true for theta in radians, false for degrees. False by default
 * @return lemlib::Pose
 */
lemlib::Pose lemlib::getLocalSpeed(bool radians) {
    if (radians) return odomLocalSpeed;
    else return lemlib::Pose(odomLocalSpeed.x, odomLocalSpeed.y, radToDeg(odomLocalSpeed.theta));
}

/**
 * @brief Estimate the pose of the robot after a certain amount of time
 *
 * @param time time in seconds
 * @param radians False for degrees, true for radians. False by default
 * @return lemlib::Pose
 */
lemlib::Pose lemlib::estimatePose(float time, bool radians) {
    // get current position and speed
    Pose curPose = getPose(true);
    Pose localSpeed = getLocalSpeed(true);
    // calculate the change in local position
    Pose deltaLocalPose = localSpeed * time;

    // calculate the future pose
    float avgHeading = curPose.theta + deltaLocalPose.theta / 2;
    Pose futurePose = curPose;
    futurePose.x += deltaLocalPose.y * sin(avgHeading);
    futurePose.y += deltaLocalPose.y * cos(avgHeading);
    futurePose.x += deltaLocalPose.x * -cos(avgHeading);
    futurePose.y += deltaLocalPose.x * sin(avgHeading);
    if (!radians) futurePose.theta = radToDeg(futurePose.theta);

    return futurePose;
}

/**
 * @brief Update the pose of the robot
=======
using namespace lemlib;

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
    if (sensors.vertical1 != nullptr) vertical1Raw = sensors.vertical1->getDistanceTraveled();
    if (sensors.vertical2 != nullptr) vertical2Raw = sensors.vertical2->getDistanceTraveled();
    if (sensors.horizontal1 != nullptr) horizontal1Raw = sensors.horizontal1->getDistanceTraveled();
    if (sensors.horizontal2 != nullptr) horizontal2Raw = sensors.horizontal2->getDistanceTraveled();
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
    if (sensors.horizontal1 != nullptr && sensors.horizontal2 != nullptr)
        heading += (deltaHorizontal1 - deltaHorizontal2) /
                   (sensors.horizontal1->getOffset() - sensors.horizontal2->getOffset());
    // else, if both vertical tracking wheels aren't substituted by the drivetrain, use the vertical tracking wheels
    else if (!sensors.vertical1->getType() && !sensors.vertical2->getType())
        heading +=
            (deltaVertical1 - deltaVertical2) / (sensors.vertical1->getOffset() - sensors.vertical2->getOffset());
    // else, if the inertial sensor exists, use it
    else if (sensors.imu != nullptr) heading += deltaImu;
    // else, use the the substituted tracking wheels
    else
        heading +=
            (deltaVertical1 - deltaVertical2) / (sensors.vertical1->getOffset() - sensors.vertical2->getOffset());
    float deltaHeading = heading - pose.theta;
    float avgHeading = pose.theta + deltaHeading / 2;
>>>>>>> ef00cb0 (Started work on odom refactor)

    // choose tracking wheels to use
    // Prioritize non-powered tracking wheels
    lemlib::TrackingWheel* verticalWheel = nullptr;
    lemlib::TrackingWheel* horizontalWheel = nullptr;
    if (!sensors.vertical1->getType()) verticalWheel = sensors.vertical1;
    else if (!sensors.vertical2->getType()) verticalWheel = sensors.vertical2;
    else verticalWheel = sensors.vertical1;
    if (sensors.horizontal1 != nullptr) horizontalWheel = sensors.horizontal1;
    else if (sensors.horizontal2 != nullptr) horizontalWheel = sensors.horizontal2;
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
