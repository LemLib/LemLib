#include "lemlib/util.hpp"

using namespace units;

namespace lemlib {

Angle angleError(Angle target, Angle position, std::optional<AngularDirection> direction) {
    // Wrap the angle to be within 0pi and 2pi radians
    target = mod(mod(target, 1_stRot) + 1_stRot, 1_stRot);

    Angle error = target - position;
    if (!direction) return from_stDeg(std::remainder(to_stDeg(error), 360));
    if (direction == AngularDirection::CW_CLOCKWISE) return error < 0_stRot ? error + 1_stRot : error;
    else return error > 0_stRot ? error - 1_stRot : error;
}

Number slew(Number target, Number current, Number maxChangeRate, Time deltaTime, SlewDirection restrictDirection) {
    if (maxChangeRate == 0) return target;

    const Number change = target - current;

    // only restrict change for specified directions
    if (restrictDirection == SlewDirection::INCREASING && change < 0) return target;
    if (restrictDirection == SlewDirection::DECREASING && change > 0) return target;

    // check if the change is within the limit
    if (abs(change) > abs(maxChangeRate * to_sec(deltaTime)))
        return current + (maxChangeRate * to_sec(deltaTime) * sgn(change));

    // return the target if no restriction is necessary
    return target;
}

Number constrainPower(Number power, Number max, Number min) {
    // respect minimum speed
    if (abs(power) < min) power = sgn(power) * min;
    // respect maximum speed
    power = std::clamp(power, -max, max);
    return power;
}

DriveOutputs desaturate(Number lateralOutput, Number angularOutput) {
    const Number left = lateralOutput - angularOutput;
    const Number right = lateralOutput + angularOutput;
    const Number sum = abs(left) + abs(right);
    if (sum <= 1.0) return {left, right};
    else return {left / sum, right / sum};
}

Curvature getSignedTangentArcCurvature(Pose start, V2Position end) {
    // whether the pose is on the left or right side of the arc
    const V2Position delta = end - start;
    const Number side = sgn(sin(start.orientation) * delta.x - cos(start.orientation) * delta.y);
    // calculate center point and radius
    const Number a = -tan(start.orientation);
    const Length c = tan(start.orientation) * start.x - start.y;
    const Length x = abs(a * end.x + end.y + c) / sqrt(a * a + 1);
    const Length d = start.distanceTo(end);
    // return the curvature
    return side * ((2 * x) / (d * d));
}

} // namespace lemlib