/**
 * @file include/lemlib/chassis/trackingWheel.hpp
 * @author LemLib Team
 * @brief tracking wheel class declarations
 * @version 0.4.5
 * @date 2023-01-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "pros/motors.hpp"
#include "pros/adi.hpp"
#include "pros/rotation.hpp"

namespace lemlib {
class TrackingWheel {
    public:
        enum class Type { POD, DRIVE };
        /**
         * @brief Create a new tracking wheel
         *
         * @param encoder the optical shaft encoder to use
         * @param diameter diameter of the tracking wheel in inches
         * @param distance distance between the tracking wheel and the center of rotation in inches
         * @param gearRatio gear ratio of the tracking wheel, defaults to 1
         */
        TrackingWheel(pros::ADIEncoder* encoder, float diameter, float distance, float gearRatio = 1);
        /**
         * @brief Create a new tracking wheel
         *
         * @param encoder the v5 rotation sensor to use
         * @param diameter diameter of the tracking wheel in inches
         * @param distance distance between the tracking wheel and the center of rotation in inches
         * @param gearRatio gear ratio of the tracking wheel, defaults to 1
         */
        TrackingWheel(pros::Rotation* encoder, float diameter, float distance, float gearRatio = 1);
        /**
         * @brief Create a new tracking wheel
         *
         * @param motors the motor group to use
         * @param diameter diameter of the drivetrain wheels in inches
         * @param distance half the track width of the drivetrain in inches
         * @param rpm theoretical maximum rpm of the drivetrain wheels
         */
        TrackingWheel(pros::Motor_Group* motors, float diameter, float distance, float rpm);
        /**
         * @brief Reset the tracking wheel position to 0
         *
         * @return bool - status of the tracking wheel. 0 if OK, 1 if error
         */
        bool reset();
        /**
         * @brief Get the distance traveled by the tracking wheel
         *
         * @return float distance traveled in inches
         */
        float getDistanceTraveled();
        /**
         * @brief Get the offset of the tracking wheel from the center of rotation
         *
         * @return float offset in inches
         */
        float getOffset();
        /**
         * @brief Get the type of tracking wheel
         *
         * @return Type type of tracking wheel (POD or DRIVE)
         */
        Type getType();
        /**
         * @brief Get the status of the tracking wheel
         *
         * @return 0 - the tracking wheel is working properly
         * @return 1 - the tracking wheel is not working properly (error)
         */
        bool getStatus();
    private:
        float diameter;
        float distance;
        float rpm;
        pros::ADIEncoder* encoder = nullptr;
        pros::Rotation* rotation = nullptr;
        pros::Motor_Group* motors = nullptr;
        float gearRatio = 1;
};
} // namespace lemlib