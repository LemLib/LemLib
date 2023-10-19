#include "lemlib/devices/encoder/encoder.hpp"

/**
 * Get the difference in angle between the current measurement of the angle and the last measurement of the angle
 */
float lemlib::Encoder::getAngleDelta() {
    float prevAngle = lastAngle; // save lastAngle, as it will get reset when calling getAngle() below
    return (getAngle() - prevAngle);
}
