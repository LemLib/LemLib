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
 * @brief A namespace representing the size of omniwheels.
 */
namespace Omniwheel {
constexpr float NEW_275 = 2.75;
constexpr float OLD_275 = 2.75;
constexpr float NEW_275_HALF = 2.744;
constexpr float OLD_275_HALF = 2.74;
constexpr float NEW_325 = 3.25;
constexpr float OLD_325 = 3.25;
constexpr float NEW_325_HALF = 3.246;
constexpr float OLD_325_HALF = 3.246;
constexpr float NEW_4 = 4;
constexpr float OLD_4 = 4.18;
constexpr float NEW_4_HALF = 3.995;
constexpr float OLD_4_HALF = 4.175;
} // namespace Omniwheel

class TrackingWheel {
    public:
        /**
         * @brief Create a new tracking wheel
         *
         * @param encoder the optical shaft encoder to use
         * @param wheelDiameter the diameter of the wheel
         * @param distance distance between the tracking wheel and the center of rotation in inches
         * @param gearRatio gear ratio of the tracking wheel, defaults to 1
         */
        TrackingWheel(pros::ADIEncoder* encoder, float wheelDiameter, float distance, float gearRatio = 1);
        /**
         * @brief Create a new tracking wheel
         *
         * @param encoder the v5 rotation sensor to use
         * @param wheelDiameter the diameter of the wheel
         * @param distance distance between the tracking wheel and the center of rotation in inches
         * @param gearRatio gear ratio of the tracking wheel, defaults to 1
         */
        TrackingWheel(pros::Rotation* encoder, float wheelDiameter, float distance, float gearRatio = 1);
        /**
         * @brief Create a new tracking wheel
         *
         * @param motors the motor group to use
         * @param wheelDiameter the diameter of the wheel
         * @param distance half the track width of the drivetrain in inches
         * @param rpm theoretical maximum rpm of the drivetrain wheels
         */
        TrackingWheel(pros::Motor_Group* motors, float wheelDiameter, float distance, float rpm);
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