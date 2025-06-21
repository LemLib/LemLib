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
#include "lemlib/chassis/distanceSensor.hpp"

// tracking thread
pros::Task* trackingTask = nullptr;

// global variables
lemlib::OdomSensors odomSensors(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                                nullptr); // the sensors to be used for odometry
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

void lemlib::setSensors(lemlib::OdomSensors sensors, lemlib::Drivetrain drivetrain) {
    odomSensors = sensors;
    drive = drivetrain;
}

lemlib::Pose lemlib::getPose(bool radians) {
    if (radians) return odomPose;
    else return lemlib::Pose(odomPose.x, odomPose.y, radToDeg(odomPose.theta));
}

void lemlib::setPose(lemlib::Pose pose, bool radians) {
    if (radians) odomPose = pose;
    else odomPose = lemlib::Pose(pose.x, pose.y, degToRad(pose.theta));
}

lemlib::Pose lemlib::getSpeed(bool radians) {
    if (radians) return odomSpeed;
    else return lemlib::Pose(odomSpeed.x, odomSpeed.y, radToDeg(odomSpeed.theta));
}

lemlib::Pose lemlib::getLocalSpeed(bool radians) {
    if (radians) return odomLocalSpeed;
    else return lemlib::Pose(odomLocalSpeed.x, odomLocalSpeed.y, radToDeg(odomLocalSpeed.theta));
}

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

// Helper: wrap angle to [0, 360)
float wrapAngle(float angle) {
    angle = fmod(angle, 360.0f);
    if (angle < 0) angle += 360.0f;
    return angle;
}

// Helper: check if angle is within a wrapped range
bool angleInRange(float angle, float start, float end) {
    angle = wrapAngle(angle);
    start = wrapAngle(start);
    end = wrapAngle(end);
    if (start < end) return angle >= start && angle <= end;
    return angle >= start || angle <= end;
}

// Helper: compute smallest angular difference
float angleDiff(float a, float b) {
    float diff = fmod((a - b + 540.0f), 360.0f) - 180.0f;
    return fabs(diff);
}
void lemlib::update() {
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
    // Priority:
    // 1. Horizontal tracking wheels
    // 2. Vertical tracking wheels
    // 3. Inertial Sensor
    // 4. Drivetrain
    float heading = odomPose.theta;
    // calculate the heading using the horizontal tracking wheels
    if (odomSensors.horizontal1 != nullptr && odomSensors.horizontal2 != nullptr)
        heading -= (deltaHorizontal1 - deltaHorizontal2) /
                   (odomSensors.horizontal1->getOffset() - odomSensors.horizontal2->getOffset());
    // else, if both vertical tracking wheels aren't substituted by the drivetrain, use the vertical tracking wheels
    else if (!odomSensors.vertical1->getType() && !odomSensors.vertical2->getType())
        heading -= (deltaVertical1 - deltaVertical2) /
                   (odomSensors.vertical1->getOffset() - odomSensors.vertical2->getOffset());
    // else, if the inertial sensor exists, use it
    else if (odomSensors.imu != nullptr) heading += deltaImu;
    // else, use the the substituted tracking wheels
    else
        heading -= (deltaVertical1 - deltaVertical2) /
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

    // save previous pose
    lemlib::Pose prevPose = odomPose;

    // calculate global x and y
    odomPose.x += localY * sin(avgHeading);
    odomPose.y += localY * cos(avgHeading);
    odomPose.x += localX * -cos(avgHeading);
    odomPose.y += localX * sin(avgHeading);
    odomPose.theta = heading;

    // calculate speed
    odomSpeed.x = ema((odomPose.x - prevPose.x) / 0.01, odomSpeed.x, 0.95);
    odomSpeed.y = ema((odomPose.y - prevPose.y) / 0.01, odomSpeed.y, 0.95);
    odomSpeed.theta = ema((odomPose.theta - prevPose.theta) / 0.01, odomSpeed.theta, 0.95);
    //odomPose.x=odomSpeed.x;

    // --- LIDAR correction alg 0.1 ---
    //if((odomSensors.distance1->getDistance()<700 || odomSensors.distance2->getDistance() <700)){
        //if (abs(odomPose.x) > 36 || abs(odomPose.y) > 36) {
            // angle is assumed to be relative to global frame or robot frame based on your system
        //     float lidarAngle = fmod(fmod(heading, 360) + 360, 360); // in deg, 0-360 wrapped
        //     float lidarX=odomPose.x;
        //     float lidarY=odomPose.y;
        //     if (odomPose.x > 36) {
        //         if (lidarAngle > 340 || lidarAngle < 20) { // right distance from right wall
        //             lidarX = 71.5 - (odomSensors.distance1->getOffset()+odomSensors.distance1->getDistance()*cos(degToRad(lidarAngle-0)));
        //         } else if (abs(lidarAngle - 180) < 20) { // left distance from right wall
        //             lidarX = 71.5 - (odomSensors.distance2->getOffset()+odomSensors.distance2->getDistance()*cos(degToRad(lidarAngle-180)));
        //         }
        //     } else if (odomPose.x < -36) {
        //         if (lidarAngle > 340 || lidarAngle < 20) { // left distance from left wall
        //             lidarX = -71.5 + (odomSensors.distance2->getOffset()+odomSensors.distance2->getDistance()*cos(degToRad(lidarAngle-0)));
        //         } else if (abs(lidarAngle - 180) < 20) { // right distance from left wall
        //             lidarX = -71.5 + (odomSensors.distance1->getOffset()+odomSensors.distance1->getDistance()*cos(degToRad(lidarAngle-180)));
        //         }
        //     }

        //     if (odomPose.y > 36) {
        //         if (abs(lidarAngle - 270) < 20) { // right distance from top wall
        //             lidarX = 71.5 - (odomSensors.distance1->getOffset()+odomSensors.distance1->getDistance()*cos(degToRad(lidarAngle-270)));
        //         } else if (abs(lidarAngle - 90) < 20) { // left distance from top wall
        //             lidarX = 71.5 - (odomSensors.distance2->getOffset()+odomSensors.distance2->getDistance()*cos(degToRad(lidarAngle-90)));
        //         }
        //     } else if (odomPose.y < -36) {
        //         if (abs(lidarAngle - 270) < 20) { // left distance from bottom wall
        //             lidarX = -71.5 + (odomSensors.distance2->getOffset()+odomSensors.distance2->getDistance()*cos(degToRad(lidarAngle-270)));
        //         } else if (abs(lidarAngle - 90) < 20) { // right distance from bottom wall
        //             lidarX = 71.5 - (odomSensors.distance1->getOffset()+odomSensors.distance1->getDistance()*cos(degToRad(lidarAngle-90)));
        //         }
        //     }
        
        // // For now, let's blend with odometry using simple averaging (or use a weighted average)
        // float alpha = 0.2; // weighting factor for blending
        
        // odomPose.x = lidarX;//alpha * lidarX + (1 - alpha) * odomPose.x;
        // odomPose.y = lidarY;//alpha * lidarY + (1 - alpha) * odomPose.y;
        //}
    //}
        
// --- LIDAR correction alg 0.2 (best version)---
    //  if((odomSensors.distance1->getDistance()<900 || odomSensors.distance2->getDistance()<900 )&&(odomSpeed.x < 10 &&odomSpeed.y < 10&&deltaHeading<5)){
        
    // //         //angle is assumed to be relative to global frame or robot frame based on system
    //         float lidarAngle = fmod(heading, 360.0f);     // Wrap within [-360, 360)
    //         if (lidarAngle < 0) lidarAngle += 360.0f;  // Wrap into [0, 360)
    //         float lidarX=odomPose.x;
    //         float lidarY=odomPose.y;
    //         if (odomPose.x > 0) {
    //             if (lidarAngle > 340 && lidarAngle < 20)  { // right distance from right wall
    //                 lidarX = 71.5 - (7.75+odomSensors.distance1->getDistance()*0.0394*cos(degToRad(lidarAngle-0)));
    //             } else if (lidarAngle > 160 && lidarAngle < 200) { // left distance from right wall
    //                 lidarX = 71.5 - (odomSensors.distance2->getOffset()+odomSensors.distance2->getDistance()*0.0394*cos(degToRad(abs(lidarAngle-180))));
    //             }
    //         } else if (odomPose.x < 0) {
    //             if (lidarAngle > 340 && lidarAngle < 20) { // left distance from left wall
    //                 lidarX = -71.5 + (odomSensors.distance2->getOffset()+odomSensors.distance2->getDistance()*0.0394*cos(degToRad(abs(lidarAngle-0))));
    //             } else if (lidarAngle > 160 && lidarAngle < 200) { // right distance from left wall
    //                 lidarX = -71.5 + (odomSensors.distance1->getOffset()+odomSensors.distance1->getDistance()*0.0394*cos(degToRad(abs(lidarAngle-180))));
    //             }
    //         }

    //         if (odomPose.y > 0) {
    //             if (lidarAngle > 250 && lidarAngle < 290) { // right distance from top wall
    //                 lidarY = 71.5 - (odomSensors.distance1->getOffset()+odomSensors.distance1->getDistance()*0.0394*cos(degToRad(abs(lidarAngle-270))));
    //             } else if (lidarAngle > 70 && lidarAngle < 110) { // left distance from top wall
    //                 lidarY = 71.5 - (odomSensors.distance2->getOffset()+odomSensors.distance2->getDistance()*0.0394*cos(degToRad(abs(lidarAngle-90))));
    //             }
    //         } else if (odomPose.y < 0) {
    //             if (lidarAngle > 250 && lidarAngle < 290) { // left distance from bottom wall
    //                 lidarY = -71.5 + (odomSensors.distance2->getOffset()+odomSensors.distance2->getDistance()*0.0394*cos(degToRad(abs(lidarAngle-270))));
    //             } else if (lidarAngle > 70 && lidarAngle < 110) { // right distance from bottom wall
    //                 lidarY = -71.5 + (odomSensors.distance1->getOffset()+odomSensors.distance1->getDistance()*0.0394*cos(degToRad(abs(lidarAngle-90))));
    //             }
    //         }
        
    //     // For now, let's blend with odometry using simple averaging (or use a weighted average)
    //     float alpha = 0.1; // weighting factor for blending
        
    //     odomPose.x = alpha * lidarX + (1 - alpha) * odomPose.x;
    //     odomPose.y = alpha * lidarY + (1 - alpha) * odomPose.y;
        
    // }
    // --- LIDAR correction alg 0.3 (cleaned and fixed) ---


    // Main correction block
    if ((odomSensors.distance1->getDistance() < 600 || odomSensors.distance2->getDistance() < 600) &&
        (fabs(odomSpeed.x) < 10 && fabs(odomSpeed.y) < 10 && fabs(deltaHeading) < 5)) {
        
        float lidarAngle = wrapAngle(heading);
        float lidarX = odomPose.x;
        float lidarY = odomPose.y;

        // Convert mm to inches
        constexpr float mmToInches = 0.0394f;

        // Correct X based on walls facing X-axis (right)
        if (angleInRange(lidarAngle, 350, 10)&&odomSensors.distance1->getDistance()<600) { // Right wall
            float dist = odomSensors.distance1->getDistance();
            lidarX = 71.5f - (odomSensors.distance1->getOffset() + dist * mmToInches * cos(degToRad(angleDiff(lidarAngle, 0))));
        }
        else if (angleInRange(lidarAngle, 170, 190)&&odomSensors.distance2->getDistance()<600) { // Right wall
            float dist = odomSensors.distance2->getDistance();
            lidarX = 71.5f - (odomSensors.distance2->getOffset() + dist * mmToInches * cos(degToRad(angleDiff(lidarAngle, 180))));
        }

        // Correct X based on walls facing X-axis (left)
        if (angleInRange(lidarAngle, 350, 10)&&odomSensors.distance2->getDistance()<600) { // Left wall
            float dist = odomSensors.distance2->getDistance();
            lidarX = -71.5f + (odomSensors.distance2->getOffset() + dist * mmToInches * cos(degToRad(angleDiff(lidarAngle, 0))));
        }
        else if (angleInRange(lidarAngle, 170, 190)&&odomSensors.distance1->getDistance()<600) { // Left wall
            float dist = odomSensors.distance1->getDistance();
            lidarX = -71.5f + (odomSensors.distance1->getOffset() + dist * mmToInches * cos(degToRad(angleDiff(lidarAngle, 180))));
        }

        // Correct Y based on walls facing Y-axis (top)
        if (angleInRange(lidarAngle, 80, 100)&&odomSensors.distance2->getDistance()<600) { // Top wall
            float dist = odomSensors.distance2->getDistance();
            lidarY = 71.5f - (odomSensors.distance2->getOffset() + dist * mmToInches * cos(degToRad(angleDiff(lidarAngle, 90))));
        }
        else if (angleInRange(lidarAngle, 260, 280)&&odomSensors.distance1->getDistance()<600) { // Top wall
            float dist = odomSensors.distance1->getDistance();
            lidarY = 71.5f - (odomSensors.distance1->getOffset() + dist * mmToInches * cos(degToRad(angleDiff(lidarAngle, 270))));
        }

        // Correct Y based on walls facing Y-axis (bottom)
        if (angleInRange(lidarAngle, 80, 100)&&odomSensors.distance1->getDistance()<600) { // Bottom wall
            float dist = odomSensors.distance1->getDistance();
            lidarY = 71.5f - (odomSensors.distance1->getOffset() + dist * mmToInches * cos(degToRad(angleDiff(lidarAngle, 90))));
        }
        else if (angleInRange(lidarAngle, 260, 280)&&odomSensors.distance2->getDistance()<600) { // Bottom wall
            float dist = odomSensors.distance2->getDistance();
            lidarY = 71.5f - (odomSensors.distance2->getOffset() + dist * mmToInches * cos(degToRad(angleDiff(lidarAngle, 270))));
        }

        // Blending: weighted average based on how large the correction is
        float dx = lidarX - odomPose.x;
        float dy = lidarY - odomPose.y;
        float correctionMagnitude = sqrt(dx * dx + dy * dy);

        float alpha = fmin(0.2f, correctionMagnitude / 24.0f);  // max alpha 0.5 for large error
        odomPose.x = alpha * lidarX + (1 - alpha) * odomPose.x;
        odomPose.y = alpha * lidarY + (1 - alpha) * odomPose.y;
    }

    // calculate local speed
    odomLocalSpeed.x = ema(localX / 0.01, odomLocalSpeed.x, 0.95);
    odomLocalSpeed.y = ema(localY / 0.01, odomLocalSpeed.y, 0.95);
    odomLocalSpeed.theta = ema(deltaHeading / 0.01, odomLocalSpeed.theta, 0.95);
}

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
