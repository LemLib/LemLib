/**
 * @file include/lemlib/chassis/chassis.hpp
 * @author LemLib Team
 * @brief Chassis class declarations
 * @version 0.4.5
 * @date 2023-01-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <functional>
#include "pros/rtos.hpp"
#include "pros/motors.hpp"
#include "pros/imu.hpp"
#include "lemlib/asset.hpp"
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
        pros::Motor_Group* leftMotors;
        pros::Motor_Group* rightMotors;
        float trackWidth;
        float wheelDiameter;
        float rpm;
        float chasePower;
} Drivetrain_t;

/**
 * @brief Function pointer type for drive curve functions.
 * @param input The control input in the range [-127, 127].
 * @param scale The scaling factor, which can be optionally ignored.
 * @return The new value to be used.
 */
typedef std::function<float(float, float)> DriveCurveFunction_t;

/**
 * @brief  Default drive curve. Modifies  the input with an exponential curve. If the input is 127, the function
 * will always output 127, no matter the value of scale, likewise for -127. This curve was inspired by team 5225, the
 * Pilons. A Desmos graph of this curve can be found here: https://www.desmos.com/calculator/rcfjjg83zx
 * @param input value from -127 to 127
 * @param scale how steep the curve should be.
 * @return The new value to be used.
 */
float defaultDriveCurve(float input, float scale);

/**
 * @brief Chassis class
 *
 */
class Chassis {
    public:
        /**
         * @brief Construct a new Chassis
         *
         * @param drivetrain drivetrain to be used for the chassis
         * @param lateralSettings settings for the lateral controller
         * @param angularSettings settings for the angular controller
         * @param sensors sensors to be used for odometry
         * @param driveCurve drive curve to be used. defaults to `defaultDriveCurve`
         */
        Chassis(Drivetrain_t drivetrain, ChassisController_t lateralSettings, ChassisController_t angularSettings,
                OdomSensors_t sensors, DriveCurveFunction_t driveCurve = &defaultDriveCurve);
        /**
         * @brief Calibrate the chassis sensors
         *
         * @param calibrateIMU whether the IMU should be calibrated. true by default
         */
        void calibrate(bool calibrateIMU = true);
        /**
         * @brief Set the pose of the chassis
         *
         * @param x new x value
         * @param y new y value
         * @param theta new theta value
         * @param radians true if theta is in radians, false if not. False by default
         */
        void setPose(float x, float y, float theta, bool radians = false);
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
        /**
         * @brief Get the speed of the robot
         *
         * @param radians true for theta in radians, false for degrees. False by default
         * @return lemlib::Pose
         */
        Pose getSpeed(bool radians = false);
        /**
         * @brief Get the local speed of the robot
         *
         * @param radians true for theta in radians, false for degrees. False by default
         * @return lemlib::Pose
         */
        Pose getLocalSpeed(bool radians = false);
        /**
         * @brief Estimate the pose of the robot after a certain amount of time
         *
         * @param time time in seconds
         * @param radians False for degrees, true for radians. False by default
         * @return lemlib::Pose
         */
        Pose estimatePose(float time, bool radians = false);
        /**
         * @brief Wait until the robot has traveled a certain distance along the path
         *
         * @note Units are in inches if curret motion is moveTo or follow, degrees if using turnTo
         *
         * @param dist the distance the robot needs to travel before returning
         */
        void waitUntilDist(float dist);
        /**
         * @brief Turn the chassis so it is facing the target point
         *
         * The PID logging id is "angularPID"
         *
         * @param x x location
         * @param y y location
         * @param timeout longest time the robot can spend moving
         * @param async whether the function should be run asynchronously. false by default
         * @param reversed whether the robot should turn to face the point with the back of the robot. false by default
         * @param maxSpeed the maximum speed the robot can turn at. Default is 200
         * @param log whether the chassis should log the turnTo function. false by default
         */
        void turnTo(float x, float y, int timeout, bool async = false, bool reversed = false, float maxSpeed = 127,
                    bool log = false);
        /**
         * @brief Move the chassis towards the target pose
         *
         * Uses the boomerang controller
         *
         * @param x x location
         * @param y y location
         * @param theta theta (in degrees). Target angle
         * @param timeout longest time the robot can spend moving
         * @param async whether the function should be run asynchronously. false by default
         * @param forwards whether the robot should move forwards or backwards. true for forwards (default), false for
         * backwards
         * @param lead the lead parameter. Determines how curved the robot will move. 0.6 by default (0 < lead < 1)
         * @param chasePower higher values make the robot move faster but causes more overshoot on turns. 0 makes it
         * default to global value
         * @param maxSpeed the maximum speed the robot can move at. 127 at default
         * @param log whether the chassis should log the turnTo function. false by default
         */
        void moveTo(float x, float y, float theta, int timeout, bool async = false, bool forwards = true,
                    float chasePower = 0, float lead = 0.6, float maxSpeed = 127, bool log = false);
        /**
         * @brief Move the chassis towards a target point
         *
         * @param x x location
         * @param y y location
         * @param timeout longest time the robot can spend moving
         * @param async whether the function should be run asynchronously. false by default
         * @param maxSpeed the maximum speed the robot can move at. 127 by default
         */
        void moveToOld(float x, float y, int timeout, bool async = false, float maxSpeed = 127);
        /**
         * @brief Move the chassis along a path
         *
         * @param filePath the filename of the path to follow
         * @param timeout the maximum time the robot can spend moving
         * @param lookahead the lookahead distance. Units in inches. Larger values will make the robot move faster
         * but will follow the path less accurately
         * @param async whether the function should be run asynchronously. false by default
         * @param forwards whether the robot should follow the path going forwards. true by default
         * @param maxSpeed the maximum speed the robot can move at
         * @param log whether the chassis should log the path on a log file. false by default.
         */
        void follow(const asset& path, int timeout, float lookahead, bool async = false, bool forwards = true,
                    float maxSpeed = 127, bool log = false);
        /**
         * @brief Control the robot during the driver control period using the tank drive control scheme. In this
         * control scheme one joystick axis controls one half of the robot, and another joystick axis controls another.
         * @param left speed of the left side of the drivetrain. Takes an input from -127 to 127.
         * @param right speed of the right side of the drivetrain. Takes an input from -127 to 127.
         * @param curveGain control how steep the drive curve is. The larger the number, the steeper the curve. A value
         * of 0 disables the curve entirely.
         */
        void tank(int left, int right, float curveGain = 0.0);
        /**
         * @brief Control the robot during the driver using the arcade drive control scheme. In this control scheme one
         * joystick axis controls the forwards and backwards movement of the robot, while the other joystick axis
         * controls  the robot's turning
         * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
         * @param turn speed to turn. Takes an input from -127 to 127.
         * @param curveGain the scale inputted into the drive curve function. If you are using the default drive
         * curve, refer to the `defaultDriveCurve` documentation.
         */
        void arcade(int throttle, int turn, float curveGain = 0.0);
        /**
         * @brief Control the robot during the driver using the curvature drive control scheme. This control scheme is
         * very similar to arcade drive, except the second joystick axis controls the radius of the curve that the
         * drivetrain makes, rather than the speed. This means that the driver can accelerate in a turn without changing
         * the radius of that turn. This control scheme defaults to arcade when forward is zero.
         * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
         * @param turn speed to turn. Takes an input from -127 to 127.
         * @param curveGain the scale inputted into the drive curve function. If you are using the default drive
         * curve, refer to the `defaultDriveCurve` documentation.
         */
        void curvature(int throttle, int turn, float cureGain = 0.0);
    private:
        pros::Mutex mutex;
        float distTravelled = 0;

        ChassisController_t lateralSettings;
        ChassisController_t angularSettings;
        Drivetrain_t drivetrain;
        OdomSensors_t odomSensors;
        DriveCurveFunction_t driveCurve;
};
} // namespace lemlib
