#include "lemlib/util.hpp"

namespace lemlib {

Angle angleError(Angle target, Angle position, AngularDirection direction) {
    // Wrap the angle to be within 0pi and 2pi radians
    target = units::mod(units::mod(target, 1_stRot) + 1_stRot, 1_stRot);

    Angle error = target - position;

    switch (direction) {
        case AngularDirection::CW_CLOCKWISE: return error < 0_stRot ? error + 1_stRot : error;
        case AngularDirection::CCW_COUNTERCLOCKWISE: return error > 0_stRot ? error - 1_stRot : error;
        default: return from_stDeg(std::remainder(to_stDeg(error), 360)); // units does not have a remainder function
    }
}

Number slew(Number target, Number current, Number maxChangeRate, Time deltaTime, SlewDirection restrictDirection) {
    using namespace units_double_ops; // enable operator overloads for Number and double

    if (maxChangeRate == 0) return target;

    const Number change = target - current;

    // only restrict change for specified directions
    if (restrictDirection == SlewDirection::INCREASING && change < 0) return target;
    if (restrictDirection == SlewDirection::DECREASING && change > 0) return target;

    // check if the change is within the limit
    if (units::abs(change) > units::abs(maxChangeRate * to_sec(deltaTime)))
        return current + (maxChangeRate * units::sgn(change));

    // return the target if no restriction is necessary
    return target;
}

Number constrainPower(Number power, Number max, Number min) {
    // respect minimum speed
    if (units::abs(power) < min) power = units::sgn(power) * min;
    // respect maximum speed
    power = std::clamp(power, -max, max);
    return power;
}
} // namespace lemlib