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
         */
        void setPose(Length x, Length y, Angle theta);

        /**
         * @brief Set the pose of the chassis
         *
         * @param pose the new pose
         */
        void setPose(Pose pose);

        /**
         * @brief Get the pose of the chassis
         *
         * @return Pose
         */
        Pose getPose() const;

        /**
         * @brief Wait until the robot has traveled a certain distance along the path
         *
         * Just uses a while loop and exits when the distance traveled is greater than the specified distance
         * or if the motion has finished
         */
        template <isQuantity Q> void waitUntil(Q dist) {
            // give the movement time to start
            pros::delay(10);
            // wait until the robot has travelled a certain distance
            while (movement != nullptr && movement->getDist() < dist.raw() && movement->getDist() >= prevDist) {
                prevDist = movement->getDist(); // update previous distance
                pros::delay(10);
            }
            // set prevDist to 0
            prevDist = 0;
        }

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
