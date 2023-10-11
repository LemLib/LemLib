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
#include "lemlib/util.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "lemlib/devices/motor.hpp"
#include "lemlib/devices/optical.hpp"
#include "lemlib/devices/rotation.hpp"

/**
 * @brief Create a tracking wheel with a custom encoder
 *
 * @param encoder unique ptr to the custom encoder to be used
 * @param diameter the diameter of the wheel, in inches
 * @param offset distance between the wheel and the tracking center, in inches
 */
lemlib::TrackingWheel::TrackingWheel(std::unique_ptr<Encoder> encoder, float diameter, float offset)
    : encoder(std::move(encoder)),
      diameter(diameter),
      offset(offset) {}

/**
 * Construct a new motor encoder tracking wheel
 *
 * We pass a pointer to a motor group instead of a reference motor group due to a
 * limitation of PROS 3. This is fixed in PROS 4, but its not ready for release yet
 */
lemlib::TrackingWheel::TrackingWheel(pros::MotorGroup* motors, float diameter, float offset, float rpm)
    : encoder(new MotorEncoder(motors, rpm)),
      diameter(diameter),
      offset(offset) {}

/**
 * Construct a new optical encoder tracking wheel
 *
 * We let the user the option to pass the ports and reversal bool directly, which
 * means they don't have to construct 2 objects for 1 tracking wheel
 */
lemlib::TrackingWheel::TrackingWheel(char topPort, char bottomPort, bool reversed, float diameter, float offset,
                                     float ratio)
    : encoder(std::make_unique<OpticalEncoder>(topPort, bottomPort, reversed, ratio)),
      diameter(diameter),
      offset(offset) {}

/**
 * Construct a new rotation sensor tracking wheel
 */
lemlib::TrackingWheel::TrackingWheel(uint8_t port, bool reversed, float diameter, float offset, float ratio)
    : encoder(std::make_unique<RotationEncoder>(port, reversed, ratio)),
      diameter(diameter),
      offset(offset) {}

/**
 * Construct a new rotation sensor tracking wheel
 *
 * We let the user pass in a signed integer for the port. Negative ports is a short form which
 * means that the rotation sensor should be reversed.
 */
lemlib::TrackingWheel::TrackingWheel(int port, float diameter, float offset, float ratio)
    : encoder(std::make_unique<RotationEncoder>(port, port < 0, ratio)),
      diameter(diameter),
      offset(offset) {}

/**
 * Reset the tracking wheel.
 */
bool lemlib::TrackingWheel::reset() { return encoder->reset(); }

/**
 * Get the distance travelled by the tracking wheel, in inches
 *
 * Since we get angle in radians, but need to convert to inches, we can simplify
 * the calculation. So, instead of writing
 * (angle / (2 * pi)) * pi * diameter
 * we do
 * (angle / 2) * diameter
 */
float lemlib::TrackingWheel::getDistance() { return encoder->getAngle() / 2 * diameter; }

/**
 * Get the offset from the tracking center, in inches
 */
float lemlib::TrackingWheel::getOffset() const { return this->offset; }

/**
 * Get the diameter of the wheel, in inches
 */
float lemlib::TrackingWheel::getDiameter() const { return this->diameter; }
