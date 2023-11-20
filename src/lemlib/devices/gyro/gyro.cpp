#include "lemlib/devices/gyro/gyro.hpp"

namespace lemlib {
/**
 * Get the difference in angle between the current measurement of the orientation and the last measurement of the
 * orientation
 */
float Gyro::getRotationDelta(bool update) {
    const float prevAngle = lastAngle; // save lastAngle, as it will get reset when calling getAngle() below
    const float angle = getRotation();
    if (!update) lastAngle = prevAngle;
    return (angle - prevAngle);
}

int Gyro::getPollRate() const {
    return pollRate;
}
}; // namespace lemlib