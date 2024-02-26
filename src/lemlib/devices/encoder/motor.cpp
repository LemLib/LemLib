#include <cmath>
#include <numeric>
#include "lemlib/devices/motor/abstractgroup.hpp"
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
MotorEncoder::MotorEncoder(std::shared_ptr<Abstract_MotorGroup> motors, float rpm)
    : motors(std::move(motors)),
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
float MotorEncoder::getAngle() {
    // get gearboxes and encoder position for each motor in the group
    std::vector<int> gearsets = this->motors->getBaseRPMs();
    std::vector<double> positions = this->motors->getPositions();
    std::vector<float> angles;
    // calculate ratio'd output for each motor
    for (int i = 0; i < this->motors->getMotorContainer().size(); i++) {
        float in = gearsets.at(i);
        angles.push_back(positions.at(i) * (rpm / in) * 2 * M_PI);
    }
    // calc average of the angles
    float angle = avg(angles);
    this->lastAngle = angle;
    return angle;
}

/**
 * Reset the motor encoders.
 */
bool MotorEncoder::reset() {
    this->lastAngle = 0;
    return (this->motors->tare_position()) ? 0 : 1;
}
}; // namespace lemlib