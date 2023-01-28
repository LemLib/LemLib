/**
 * @file lemlib/chassis/trackingWheel.hpp
 * @author Liam Teale
 * @brief tracking wheel class declarations
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once


#include "pros/adi.hpp"
#include "pros/rotation.hpp"


namespace lemlib {
    class TrackingWheel {
        public:
            /**
             * @brief Create a new tracking wheel
             * 
             * @param encoder the optical shaft encoder to use
             * @param diameter diameter of the tracking wheel in inches
             * @param distance distance between the tracking wheel and the center of rotation in inches
             */
            TrackingWheel(pros::ADIEncoder *encoder, float diameter, float distance);
            /**
             * @brief Create a new tracking wheel
             * 
             * @param encoder the v5 rotation sensor to use
             * @param diameter diameter of the tracking wheel in inches
             * @param distance distance between the tracking wheel and the center of rotation in inches
             */
            TrackingWheel(pros::Rotation *encoder, float diameter, float distance);
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
        private:
            float diameter;
            float distance;
            pros::ADIEncoder *encoder = nullptr;
            pros::Rotation *rotation = nullptr;
    };
}