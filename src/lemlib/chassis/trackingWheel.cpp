
#include <math.h>
#include "lemlib/chassis/trackingWheel.hpp"
#include "lemlib/util.hpp"
#include "pros/llemu.hpp"

/**
 * @brief Create a new tracking wheel
 *
 * @param encoder the optical shaft encoder to use
 * @param wheelDiameter the diameter of the wheel
 * @param distance distance between the tracking wheel and the center of rotation in inches
 * @param gearRatio gear ratio of the tracking wheel, defaults to 1
 */
lemlib::TrackingWheel::TrackingWheel(pros::ADIEncoder* encoder, float wheelDiameter, float distance, float gearRatio) {
    this->encoder = encoder;
    this->diameter = wheelDiameter;
    this->distance = distance;
    this->gearRatio = gearRatio;
}

/**
 * @brief Create a new tracking wheel
 *
 * @param encoder the v5 rotation sensor to use
 * @param wheelDiameter the diameter of the wheel
 * @param distance distance between the tracking wheel and the center of rotation in inches
 * @param gearRatio gear ratio of the tracking wheel, defaults to 1
 */
lemlib::TrackingWheel::TrackingWheel(pros::Rotation* encoder, float wheelDiameter, float distance, float gearRatio) {
    this->rotation = encoder;
    this->diameter = wheelDiameter;
    this->distance = distance;
    this->gearRatio = gearRatio;
}

/**
 * @brief Create a new tracking wheel
 *
 * @param motors the motor group to use
 * @param wheelDiameter the diameter of the wheel
 * @param distance half the track width of the drivetrain in inches
 * @param rpm theoretical maximum rpm of the drivetrain wheels
 */
lemlib::TrackingWheel::TrackingWheel(pros::Motor_Group* motors, float wheelDiameter, float distance, float rpm) {
    this->motors = motors;
    this->motors->set_encoder_units(pros::E_MOTOR_ENCODER_ROTATIONS);
    this->diameter = wheelDiameter;
    this->distance = distance;
    this->rpm = rpm;
}

/**
 * @brief Reset the tracking wheel position to 0
 *
 */
void lemlib::TrackingWheel::reset() {
    if (this->encoder != nullptr) this->encoder->reset();
    if (this->rotation != nullptr) this->rotation->reset_position();
    if (this->motors != nullptr) this->motors->tare_position();
}

/**
 * @brief Get the distance traveled by the tracking wheel
 *
 * @return float distance traveled in inches
 */
float lemlib::TrackingWheel::getDistanceTraveled() {
    if (this->encoder != nullptr) {
        return (float(this->encoder->get_value()) * this->diameter * M_PI / 360) / this->gearRatio;
    } else if (this->rotation != nullptr) {
        return (float(this->rotation->get_position()) * this->diameter * M_PI / 36000) / this->gearRatio;
    } else if (this->motors != nullptr) {
        // get distance traveled by each motor
        std::vector<pros::motor_gearset_e_t> gearsets = this->motors->get_gearing();
        std::vector<double> positions = this->motors->get_positions();
        std::vector<float> distances;
        for (int i = 0; i < this->motors->size(); i++) {
            float in;
            switch (gearsets[i]) {
                case pros::E_MOTOR_GEARSET_36: in = 100; break;
                case pros::E_MOTOR_GEARSET_18: in = 200; break;
                case pros::E_MOTOR_GEARSET_06: in = 600; break;
                default: in = 200; break;
            }
            distances.push_back(positions[i] * (diameter * M_PI) * (rpm / in));
        }
        return lemlib::avg(distances);
    } else {
        return 0;
    }
}

/**
 * @brief Get the offset of the tracking wheel from the center of rotation
 *
 * @return float offset in inches
 */
float lemlib::TrackingWheel::getOffset() { return this->distance; }

/**
 * @brief Get the type of tracking wheel
 *
 * @return int - 1 if motor group, 0 otherwise
 */
int lemlib::TrackingWheel::getType() {
    if (this->motors != nullptr) return 1;
    return 0;
}
