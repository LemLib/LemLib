#include <math.h>
#include "lemlib/chassis/trackingWheel.hpp"
#include "lemlib/util.hpp"
#include "pros/llemu.hpp"

lemlib::TrackingWheel::TrackingWheel(pros::ADIEncoder* encoder, float wheelDiameter, float distance, float gearRatio) {
    this->encoder = encoder;
    this->diameter = wheelDiameter;
    this->distance = distance;
    this->gearRatio = gearRatio;
}

lemlib::TrackingWheel::TrackingWheel(pros::Rotation* encoder, float wheelDiameter, float distance, float gearRatio) {
    this->rotation = encoder;
    this->diameter = wheelDiameter;
    this->distance = distance;
    this->gearRatio = gearRatio;
}

lemlib::TrackingWheel::TrackingWheel(pros::Motor_Group* motors, float wheelDiameter, float distance, float rpm) {
    this->motors = motors;
    this->motors->set_encoder_units(pros::E_MOTOR_ENCODER_ROTATIONS);
    this->diameter = wheelDiameter;
    this->distance = distance;
    this->rpm = rpm;
}

void lemlib::TrackingWheel::reset() {
    if (this->encoder != nullptr) this->encoder->reset();
    if (this->rotation != nullptr) this->rotation->reset_position();
    if (this->motors != nullptr) this->motors->tare_position();
}

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

float lemlib::TrackingWheel::getOffset() { return this->distance; }

int lemlib::TrackingWheel::getType() {
    if (this->motors != nullptr) return 1;
    return 0;
}
