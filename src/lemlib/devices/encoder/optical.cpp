#include <cmath>
#include "lemlib/devices/encoder/optical.hpp"

namespace lemlib {
/**
 * Construct a new Optical Encoder
 *
 * This is a derived class of the Encoder abstract class.
 * This abstraction is easy as there is only a single sensor which needs to be
 * checked.
 */
OpticalEncoder::OpticalEncoder(char topPort, char bottomPort, bool reversed, float ratio)
    : optical(pros::adi::Encoder(topPort, bottomPort, reversed)),
      ratio(ratio) {}

/**
 * Get the angle the optical encoder rotated by, in radians
 *
 * Pretty straightforward, raw value from the encoder gets converted to rotations
 * which gets converted to radians
 */
float OpticalEncoder::getAngle() {
    float angle = (float(optical.get_value()) * (2 * M_PI) / 360) / ratio;
    lastAngle = angle;
    return angle;
}

/**
 * Reset/calibrate the optical encoder
 */
bool OpticalEncoder::reset() { return (optical.reset()) ? 0 : 1; }
}; // namespace lemlib