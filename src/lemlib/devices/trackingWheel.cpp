/**
 * @file src/lemlib/chassis/trackingWheel.cpp
 * @author LemLib Team
 * @brief tracking wheel class definitions
 * @version 0.4.5
 * @date 2023-01-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <math.h>
#include "lemlib/units.hpp"
#include "lemlib/util.hpp"
#include "lemlib/devices/trackingWheel.hpp"
#include "lemlib/devices/encoder/motor.hpp"
#include "lemlib/devices/encoder/optical.hpp"
#include "lemlib/devices/encoder/rotation.hpp"

namespace lemlib {
/**
 * @brief Create a tracking wheel with a custom encoder
 *
 * @param encoder unique ptr to the custom encoder to be used
 * @param diameter the diameter of the wheel, in inches
 * @param offset distance between the wheel and the tracking center, in inches
 */
TrackingWheel::TrackingWheel(std::shared_ptr<Encoder> encoder, Length diameter, Length offset)
    : encoder(encoder),
      diameter(diameter),
      offset(offset) {}

/**
 * Construct a new motor encoder tracking wheel
 *
 * We pass a pointer to a motor group instead of a reference motor group due to a
 * limitation of PROS 3. This is fixed in PROS 4, but its not ready for release yet
 */
TrackingWheel::TrackingWheel(std::shared_ptr<pros::MotorGroup> motors, Length diameter, Length offset, AngularVelocity rpm)
    : encoder(std::make_shared<MotorEncoder>(MotorEncoder(motors, rpm))),
      diameter(diameter),
      offset(offset) {}

/**
 * Construct a new optical encoder tracking wheel
 *
 * We let the user the option to pass the ports and reversal bool directly, which
 * means they don't have to construct 2 objects for 1 tracking wheel
 */
TrackingWheel::TrackingWheel(char topPort, char bottomPort, bool reversed, Length diameter, Length offset, float ratio)
    : encoder(std::make_shared<OpticalEncoder>(topPort, bottomPort, reversed, ratio)),
      diameter(diameter),
      offset(offset) {}

/**
 * Construct a new rotation sensor tracking wheel
 */
TrackingWheel::TrackingWheel(uint8_t port, bool reversed, Length diameter, Length offset, float ratio)
    : encoder(std::make_shared<RotationEncoder>(port, reversed, ratio)),
      diameter(diameter),
      offset(offset) {}

/**
 * Construct a new rotation sensor tracking wheel
 *
 * We let the user pass in a signed integer for the port. Negative ports is a short form which
 * means that the rotation sensor should be reversed.
 */
TrackingWheel::TrackingWheel(int port, Length diameter, Length offset, float ratio)
    : encoder(std::make_shared<RotationEncoder>(port, port < 0, ratio)),
      diameter(diameter),
      offset(offset) {}

/**
 * Reset the tracking wheel.
 */
bool TrackingWheel::reset() { return encoder->reset(); }

/**
 * Get the distance travelled by the tracking wheel, in inches
 *
 * Since we get angle in radians, but need to convert to inches, we can simplify
 * the calculation. So, instead of writing
 * (angle / (2 * pi)) * pi * diameter
 * we do
 * (angle / 2) * diameter
 */
Length TrackingWheel::getDistance() { return encoder->getAngle() / 2_rad * diameter; }

/**
 * Get the difference in distance travelled by the tracking wheel, in inches
 *
 * Since we get angle in radians, but need to convert to inches, we can simplify
 * the calculation. So, instead of writing
 * (angle / (2 * pi)) * pi * diameter
 * we do
 * (angle / 2) * diameter
 */
Length TrackingWheel::getDistanceDelta(bool update) { return encoder->getAngleDelta(update) / 2_rad * diameter; }

/**
 * Get the offset from the tracking center
 */
Length TrackingWheel::getOffset() const { return this->offset; }

/**
 * Get the diameter of the wheel, in inches
 */
Length TrackingWheel::getDiameter() const { return this->diameter; }
}; // namespace lemlib