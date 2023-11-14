#include "lemlib/devices/gyro/gyro.hpp"

namespace lemlib {
/**
 * Get the difference in angle between the current measurement of the orientation and the last measurement of the
 * orientation
 */
Angle Gyro::getRotationDelta(bool update) {
    const Angle prevAngle = lastAngle; // save lastAngle, as it will get reset when calling getAngle() below
    const Angle angle = getRotation();
    if (!update) lastAngle = prevAngle;
    return (angle - prevAngle);
}
}; // namespace lemlib