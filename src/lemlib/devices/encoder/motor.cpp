#include <cmath>
#include <numeric>
#include "lemlib/units.hpp"
#include "lemlib/util.hpp"
#include "lemlib/devices/encoder/motor.hpp"

namespace lemlib {
/**
 * Construct a new motor encoder object
 *
 * This is a derived class of the Encoder abstract class.
 * Currently, a pointer to a MotorGroup is passed instead of just using
 * a reference, due to limitations in PROS 3. This is fixed in PROS 4, but
 * we have to deal with this for now.
 */
MotorEncoder::MotorEncoder(std::shared_ptr<pros::MotorGroup> motors, AngularVelocity rpm)
    : motors(std::move(motors)),
      speed(rpm) {}

/**
 * Get the angle the motors rotated by
 *
 * Since the motors in the group may have different cartridges, we need some
 * extra logic to calculate the geared output. All we do is get a vector
 * of all the gearboxes, and calculate the appropriate ratio by diving the
 * output rpm by the input rpm. Then we just multiply the output by 2 pi
 * to get angle in radians.
 */
Angle MotorEncoder::getAngle() {
    // get gearboxes and encoder position for each motor in the group
    std::vector<pros::MotorGears> gearsets = motors->get_gearing_all();
    std::vector<double> positions = motors->get_position_all();
    std::vector<Angle> angles;
    // calculate ratio'd output for each motor
    for (int i = 0; i < motors->size(); i++) {
        AngularVelocity inp;
        switch (gearsets[i]) {
            case pros::MotorGears::rpm_100: inp = 100_rpm; break;
            case pros::MotorGears::rpm_200: inp = 200_rpm; break;
            case pros::MotorGears::rpm_600: inp = 600_rpm; break;
            default: inp = 200_rpm; break;
        }
        angles.push_back(positions[i] * (speed / inp) * 1_rot); // todo test
    }
    // calc average of the angles
    Angle angle = 0_deg;
    for (Angle value : angles) { angle += value; }
    angle /= angles.size();
    lastAngle = angle;
    return angle;
}

/**
 * Reset the motor encoders.
 */
bool MotorEncoder::reset() {
    lastAngle = 0_deg;
    return (motors->tare_position()) ? 0 : 1;
}
}; // namespace lemlib