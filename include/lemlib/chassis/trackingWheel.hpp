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

/**
 * @brief An omni-wheel enumeration.
 *
 * @note These enumerations are equal to the virtual diameter of a wheel which would have a circumference equal to the
 * real loop length of the given wheel in thous.
 *
 */
enum class Omniwheel {
    NEW_275 = 2750,
    OLD_275 = 2750,
    NEW_275_HALF = 2744,
    OLD_275_HALF = 2740,
    NEW_325 = 3250,
    OLD_325 = 3250,
    NEW_325_HALF = 3246,
    OLD_325_HALF = 3246,
    NEW_4 = 4000,
    OLD_4 = 4180,
    NEW_4_HALF = 3995,
    OLD_4_HALF = 4175
};

class TrackingWheel {
    public:
        /**
         * @brief Create a new tracking wheel
         *
         * @param encoder the optical shaft encoder to use
         * @param wheel the omni-wheel to use
         * @param distance distance between the tracking wheel and the center of rotation in inches
         * @param gearRatio gear ratio of the tracking wheel, defaults to 1
         */
        TrackingWheel(pros::ADIEncoder* encoder, Omniwheel wheel, float distance, float gearRatio = 1);
        /**
         * @brief Create a new tracking wheel
         *
         * @param encoder the v5 rotation sensor to use
         * @param wheel the omni-wheel to use
         * @param distance distance between the tracking wheel and the center of rotation in inches
         * @param gearRatio gear ratio of the tracking wheel, defaults to 1
         */
        TrackingWheel(pros::Rotation* encoder, Omniwheel wheel, float distance, float gearRatio = 1);
        /**
         * @brief Create a new tracking wheel
         *
         * @param motors the motor group to use
         * @param wheel the omni-wheel to use
         * @param distance half the track width of the drivetrain in inches
         * @param rpm theoretical maximum rpm of the drivetrain wheels
         */
        TrackingWheel(pros::Motor_Group* motors, Omniwheel wheel, float distance, float rpm);
        /**
         * @brief Reset the tracking wheel position to 0
         *
         */
        void reset();
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
         * @return int - 1 if motor group, 0 otherwise
         */
        int getType();
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