/**
 * @file lemlib/chassis/trackingWheel.cpp
 * @author Liam Teale
 * @brief tracking wheel class definitions
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <math.h>
#include "lemlib/chassis/trackingWheel.hpp"


/**
 * @brief Create a new tracking wheel
 * 
 * @param encoder the optical shaft encoder to use
 * @param diameter diameter of the tracking wheel in inches
 * @param distance distance between the tracking wheel and the center of rotation in inches
 */
lemlib::TrackingWheel::TrackingWheel(pros::ADIEncoder *encoder, float diameter, float distance) {
    this->encoder = encoder;
    this->diameter = diameter;
    this->distance = distance;
}


/**
 * @brief Create a new tracking wheel
 * 
 * @param encoder the v5 rotation sensor to use
 * @param diameter diameter of the tracking wheel in inches
 * @param distance distance between the tracking wheel and the center of rotation in inches
 */
lemlib::TrackingWheel::TrackingWheel(pros::Rotation *encoder, float diameter, float distance) {
    this->rotation = encoder;
    this->diameter = diameter;
    this->distance = distance;
}


/**
 * @brief Get the distance traveled by the tracking wheel
 *
 * @return float distance traveled in inches
 */
float lemlib::TrackingWheel::getDistanceTraveled() {
    if (this->encoder != nullptr) {
        return float(this->encoder->get_value()) * this->diameter * M_PI / 360;
    } else if (this->rotation != nullptr) {
        return float(this->rotation->get_position()) * this->diameter * M_PI / 36000;
    } else {
        return 0;
    }
}


/**
 * @brief Get the offset of the tracking wheel from the center of rotation
 * 
 * @return float offset in inches
 */
float lemlib::TrackingWheel::getOffset() {
    return this->distance;
}
