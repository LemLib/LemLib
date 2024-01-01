#include "lemlib/devices/encoder/encoder.hpp"

namespace lemlib {
/**
 * Get the difference in angle between the current measurement of the angle and the last measurement of the angle
 */
float Encoder::getAngleDelta(bool update) {
    const float prevAngle = this->lastAngle; // save lastAngle, as it will get reset when calling getAngle() below
    const float angle = getAngle();
    if (!update) this->lastAngle = prevAngle;
    return (angle - prevAngle);
}
}; // namespace lemlib