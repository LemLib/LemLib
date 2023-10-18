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

#include <memory>
#include "pros/rtos.hpp"
#include "lemlib/pose.hpp"
#include "lemlib/movements/movement.hpp"
#include "lemlib/odom/odom.hpp"

namespace lemlib {
/**
 * @brief Chassis class
 *
 */
class Chassis {
    public:
        /**
         * @brief Initialize the chassis
         *
         */
        virtual void initialize() = 0;

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
         * @brief Wait until the robot has traveled a certain distance, or angle, along the path
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
         * @note Units are in inches if current motion is moveTo or follow, degrees if using turnTo
         *
         * @param dist the distance the robot needs to travel before returning
         */
        void waitUntil(float dist);
        /**
         * @brief Turn the chassis so it is facing the target point
         *
         * The PID logging id is "angularPID"
         *
         * @param x x location
         * @param y y location
         * @param timeout longest time the robot can spend moving
         * @param forwards whether the robot should turn to face the point with the front of the robot. true by default
         * @param maxSpeed the maximum speed the robot can turn at. Default is 127
         * @param async whether the function should be run asynchronously. true by default
         */
        void turnTo(float x, float y, int timeout, bool forwards = true, float maxSpeed = 127, bool async = true);
        /**
         * @brief Move the chassis towards the target pose
         *
         * Uses the boomerang controller
         *
         * @param x x location
         * @param y y location
         * @param theta target heading in degrees.
         * @param timeout longest time the robot can spend moving
         * @param forwards whether the robot should move forwards or backwards. true for forwards (default), false for
         * backwards
         * @param chasePower higher values make the robot move faster but causes more overshoot on turns. 0 makes it
         * default to global value
         * @param lead the lead parameter. Determines how curved the robot will move. 0.6 by default (0 < lead < 1)
         * @param maxSpeed the maximum speed the robot can move at. 127 at default
         * @param async whether the function should be run asynchronously. true by default
         */
        void moveTo(float x, float y, float theta, int timeout, bool forwards = true, float chasePower = 0,
                    float lead = 0.6, float maxSpeed = 127, bool async = false);
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
         * @param async whether the function should be run asynchronously. true by default
         */
        void follow(const asset& path, float lookahead, int timeout, bool forwards = true, bool async = true);
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

        /**
         * @brief Wait until the current movement is done
         *
         */
        void waitUntilDone();

        /**
         * @brief Move the chassis using a custom motion algorithm
         *
         * @param movement shared pointer to the custom movement
         */
        void moveCustom(std::unique_ptr<Movement> movement);
    protected:
        /**
         * @brief Chassis update function. Updates chassis motion and odometry
         *
         */
        virtual void update() = 0;

        float prevDist = 0; // the previous distance travelled by the movement

        std::unique_ptr<Odom> odom = nullptr;
        std::unique_ptr<Movement> movement = nullptr;
        std::unique_ptr<pros::Task> task = nullptr;
};
} // namespace lemlib
