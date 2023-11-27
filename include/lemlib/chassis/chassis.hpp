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
        Pose getPose(bool radians = false) const;

        /**
         * @brief Wait until the robot has traveled a certain distance along the path
         *
         * @note Units are in inches if current motion is moveTo or follow, degrees if using turnTo
         *
         * @param dist the distance the robot needs to travel before returning
         */
        void waitUntil(float dist);

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
