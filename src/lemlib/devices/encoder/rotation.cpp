#include <cmath>
#include "lemlib/util.hpp"
#include "lemlib/devices/encoder/rotation.hpp"

namespace lemlib {
/**
 * Construct a new Rotation Encoder
 *
 * This is a class derived from the Encoder class.
 * This abstraction is pretty easy because there is only 1 sensor that needs
 * to be checked.
 *
 * Because of a missing parameter in the rotation sensor constructor, we have to do
 * some wacky stuff in the constructor so our users still have the choice of a flag
 */
RotationEncoder::RotationEncoder(int port, bool reversed, float ratio)
    : rotation(pros::Rotation(int(port) * sgn(float(!reversed) - 0.1))),
      ratio(ratio) {}

/**
 * Get the angle the rotation sensor rotated by, in radians
 *
 * Pretty straightforward, raw value from the rotation sensor gets converted to rotations
 * which gets converted to radians
 */
float RotationEncoder::getAngle() {
    const float angle = (float(this->rotation.get_position()) / 36000) * (2 * M_PI) / this->ratio;
    this->lastAngle = angle;
    return angle;
}

/**
 * Reset/calibrate the optical encoder
 */
bool RotationEncoder::reset() {
    this->lastAngle = 0;
    return (this->rotation.reset_position()) ? 0 : 1;
}
}; // namespace lemlib