#pragma once

#include "pros/motor_group.hpp"
#include "pros/imu.hpp"
#include "lemlib/devices/trackingWheel.hpp"

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
        TrackingWheel* vertical1;
        TrackingWheel* vertical2;
        TrackingWheel* horizontal1;
        TrackingWheel* horizontal2;
        pros::Imu* imu;
} OdomSensors_t;

/**
 * @brief Struct containing constants for a chassis controller
 *
 * The constants are stored in a struct so that they can be easily passed to the chassis class
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
typedef struct {
        float kP;
        float kD;
        float smallError;
        float smallErrorTimeout;
        float largeError;
        float largeErrorTimeout;
        float slew;
} ChassisController_t;

/**
 * @brief Struct containing constants for a drivetrain
 *
 * The constants are stored in a struct so that they can be easily passed to the chassis class
 * Set a constant to 0 and it will be ignored
 *
 * @param leftMotors pointer to the left motors
 * @param rightMotors pointer to the right motors
 * @param trackWidth the track width of the robot
 * @param wheelDiameter the diameter of the wheel used on the drivetrain
 * @param rpm the rpm of the wheels
 * @param chasePower higher values make the robot move faster but causes more overshoot on turns
 */
typedef struct {
        pros::MotorGroup* leftMotors;
        pros::MotorGroup* rightMotors;
        float trackWidth;
        float wheelDiameter;
        float rpm;
        float chasePower;
} Drivetrain_t;
} // namespace lemlib