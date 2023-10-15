#include <cmath>
#include "lemlib/util.hpp"
#include "lemlib/devices/encoder/motor.hpp"

/**
 * Construct a new motor encoder object
 *
 * This is a derived class of the Encoder abstract class.
 * Currently, a pointer to a MotorGroup is passed instead of just using
 * a reference, due to limitations in PROS 3. This is fixed in PROS 4, but
 * we have to deal with this for now.
 */
lemlib::MotorEncoder::MotorEncoder(pros::MotorGroup* motors, float rpm)
    : motors(motors),
      rpm(rpm) {}

/**
 * Get the angle the motors rotated by, in radians
 *
 * Since the motors in the group may have different cartridges, we need some
 * extra logic to calculate the geared output. All we do is get a vector
 * of all the gearboxes, and calculate the appropriate ratio by diving the
 * output rpm by the input rpm. Then we just multiply the output by 2 pi
 * to get angle in radians.
 */
float lemlib::MotorEncoder::getAngle() {
    // get gearboxes and encoder position for each motor in the group
    std::vector<pros::motor_gearset_e_t> gearsets = this->motors->get_gearing();
    std::vector<double> positions = this->motors->get_positions();
    std::vector<float> angles;
    // calculate ratio'd output for each motor
    for (int i = 0; i < this->motors->size(); i++) {
        float in;
        switch (gearsets[i]) {
            case pros::E_MOTOR_GEARSET_36: in = 100; break;
            case pros::E_MOTOR_GEARSET_18: in = 200; break;
            case pros::E_MOTOR_GEARSET_06: in = 600; break;
            default: in = 200; break;
        }
        angles.push_back(positions[i] * (rpm / in) * 2 * M_PI);
    }
    // return average of elements in the angles vector
    return avg(angles);
}

/**
 * Reset the motor encoders.
 */
bool lemlib::MotorEncoder::reset() { return (motors->tare_position()) ? 0 : 1; }
