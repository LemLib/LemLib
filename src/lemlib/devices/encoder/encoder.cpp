#include "lemlib/devices/encoder/encoder.hpp"

/**
 * Get the difference in angle between the current measurement of the angle and the last measurement of the angle
 */
float lemlib::Encoder::getAngleDelta(bool update) {
    const float prevAngle = lastAngle; // save lastAngle, as it will get reset when calling getAngle() below
    const float angle = getAngle();
    if (!update) lastAngle = prevAngle;
    return (angle - prevAngle);
}
