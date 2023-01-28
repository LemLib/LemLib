/**
 * @file lemlib/chassis/chassis.hpp
 * @author Liam Teale
 * @brief Chassis class declarations
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include "pros/motors.hpp"
#include "pros/imu.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "lemlib/pose.hpp"

namespace lemlib {
    /**
     * @brief Struct containing all the sensors used for odometry
     * 
     * The sensors are stored in a struct so that they can be easily passed to the chassis class
     * The variables are pointers so that they can be set to nullptr if they are not used
     * Otherwise the chassis class would have to have a constructor for each possible combination of sensors
     *
     * @param vertical1 pointer to the first vertical tracking wheel
     * @param vertical2 pointer to the second vertical tracking wheel
     * @param horizontal1 pointer to the first horizontal tracking wheel
     * @param horizontal2 pointer to the second horizontal tracking wheel
     * @param imu pointer to the IMU
     */
    typedef struct {
        TrackingWheel *vertical1;
        TrackingWheel *vertical2;
        TrackingWheel *horizontal1;
        TrackingWheel *horizontal2;
        pros::Imu *imu;
    } OdomSensors_t;

    /**
     * @brief Chassis class
     * 
     */
    class Chassis {
        public:
            /**
             * @brief Construct a new Chassis
             * 
             * @param leftMotors motors on the left side of the drivetrain
             * @param rightMotors motors on the right side of the drivetrain
             * @param sensors sensors to be used for odometry
             */
            Chassis(pros::Motor_Group *leftMotors, pros::Motor_Group *rightMotors, OdomSensors_t sensors);
            /**
             * @brief Calibrate the chassis sensors
             * 
             */
            void calibrate();
            /**
             * @brief Set the pose of the chassis
             * 
             * @param x new x value
             * @param y new y value
             * @param theta new theta value
             * @param radians true if theta is in radians, false if not. False by default
             */
            void setPose(double x, double y, double theta, bool radians = false);
            /**
             * @brief Set the pose of the chassis 
             *
             * @param pose the new pose
             * @param radians whether pose theta is in radians (true) or not (false). false by default
             */
            void setPose(Pose pose, bool radians = false);
            /**
             * @brief Get the pose of the chassis
             * 
             * @param radians whether theta should be in radians (true) or degrees (false). false by default
             * @return Pose 
             */
            Pose getPose(bool radians = false);
        private:
            OdomSensors_t odomSensors;
            pros::Motor_Group *leftMotorGroup;
            pros::Motor_Group *rightMotorGroup;
    };
}
