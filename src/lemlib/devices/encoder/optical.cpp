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
 * Get the angle the optical encoder rotated by
 *
 * Pretty straightforward, raw value from the encoder gets converted to rotations
 * which gets converted to radians
 */
Angle OpticalEncoder::getAngle() {
    Angle angle = (float(optical.get_value()) * 1_deg) / ratio;
    lastAngle = angle;
    return angle;
}

/**
 * Reset/calibrate the optical encoder
 */
bool OpticalEncoder::reset() { return (optical.reset()) ? 0 : 1; }
}; // namespace lemlib