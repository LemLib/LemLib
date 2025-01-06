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

double lemlib::getCurvature(units::Pose pose, units::Pose other) {
    // calculate whether the pose is on the left or right side of the circle
    double side = units::sgn(units::sin(pose.orientation) * (other.x - pose.x) -
                             units::cos(pose.orientation) * (other.y - pose.y));
    // calculate center point and radius
    Number a = -units::tan(pose.orientation);
    Number c = units::tan(pose.orientation) * pose.x.convert(in) - pose.y.convert(in);
    Number x = units::abs(a * other.x.convert(in) - other.y.convert(in) + c) / units::sqrt(units::square(a) + 1);
    Number d = units::hypot(other.x - pose.x, other.y - pose.y).convert(in);

    return (side * ((2 * x) / units::square(d))).internal();
}

std::tuple<double, double> lemlib::ratioSpeeds(double lateral, double angular, double maxSpeed) {
    double leftPower = lateral + angular;
    double rightPower = lateral - angular;
    const double ratio = std::max(std::fabs(leftPower), std::fabs(rightPower)) / maxSpeed;
    if (ratio > 1) {
        leftPower /= ratio;
        rightPower /= ratio;
    }

    return {leftPower, rightPower};
}