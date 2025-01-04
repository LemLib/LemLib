#include "lemlib/util.hpp"

Angle lemlib::angleError(Angle target, Angle position, lemlib::AngularDirection direction) {
    // Wrap the angle to be within 0pi and 2pi radians
    target = units::mod(units::mod(target, 1_stRot) + 1_stRot, 1_stRot);

    Angle error = target - position;

    switch (direction) {
        case lemlib::AngularDirection::CW_CLOCKWISE: return error < 0_stRot ? error + 1_stRot : error;
        case lemlib::AngularDirection::CCW_COUNTERCLOCKWISE: return error > 0_stRot ? error - 1_stRot : error;
        default: return std::remainder(error.convert(deg), 360) * deg; // units does not have a remainder function
    }
}

double lemlib::slew(double target, double current, double maxChange) {
    double change = target - current;
    if (maxChange == 0) return target;
    if (change > maxChange) change = maxChange;
    else if (change < -maxChange) change = -maxChange;
    return current + change;
}

double lemlib::respectSpeeds(double power, double previous, double max, double min, double slew) {
    // respect maximum speed
    if (power > max) power = max;
    else if (power < -max) power = -max;

    // optionally slew if enabled
    if (slew != 0) power = lemlib::slew(power, previous, slew);

    // respect minimum speed
    if (power < 0 && power > -min) power = -min;
    else if (power > 0 && power < min) power = min;

    return power;
}