#include <cmath>
#include "lemlib/util.hpp"
#include "lemlib/devices/encoder/rotation.hpp"

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
lemlib::RotationEncoder::RotationEncoder(int port, bool reversed, float ratio)
    : rotation(pros::Rotation(int(port) * sgn(float(!reversed) - 0.1))),
      ratio(ratio) {}

/**
 * Get the angle the rotation sensor rotated by, in radians
 *
 * Pretty straightforward, raw value from the rotation sensor gets converted to rotations
 * which gets converted to radians
 */
float lemlib::RotationEncoder::getAngle() {
    float angle = (float(rotation.get_position()) / 36000) * (2 * M_PI) / ratio;
    lastAngle = angle;
    return angle;
}

/**
 * Reset/calibrate the optical encoder
 */
bool lemlib::RotationEncoder::reset() {
    lastAngle = 0;
    return (rotation.reset_position()) ? 0 : 1;
}
