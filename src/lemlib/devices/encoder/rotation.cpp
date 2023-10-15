#include <cmath>
#include "lemlib/devices/encoder/rotation.hpp"

/**
 * Construct a new Rotation Encoder
 *
 * This is a class derived from the Encoder class.
 * This abstraction is pretty easy because there is only 1 sensor that needs
 * to be checked.
 */
lemlib::RotationEncoder::RotationEncoder(int port, bool reversed, float ratio)
    : rotation(pros::Rotation(port, reversed)),
      ratio(ratio) {}

/**
 * Get the angle the rotation sensor rotated by, in radians
 *
 * Pretty straightforward, raw value from the rotation sensor gets converted to rotations
 * which gets converted to radians
 */
float lemlib::RotationEncoder::getAngle() { return (float(rotation.get_position()) / 36000) * (2 * M_PI) / ratio; }

/**
 * Reset/calibrate the optical encoder
 */
bool lemlib::RotationEncoder::reset() { return (rotation.reset_position()) ? 0 : 1; }
