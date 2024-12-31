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