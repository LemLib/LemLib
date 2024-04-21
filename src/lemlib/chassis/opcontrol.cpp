#include "lemlib/chassis/chassis.hpp"
#include "lemlib/util.hpp"
#include <math.h>

namespace lemlib {

ExpoDriveCurve defaultDriveCurve = ExpoDriveCurve(0, 0, 1);

void Chassis::tank(int left, int right, bool disableDriveCurve) {
    if (disableDriveCurve) {
        drivetrain.leftMotors->move(left);
        drivetrain.rightMotors->move(right);
    } else {
        drivetrain.leftMotors->move(throttleCurve->curve(left));
        drivetrain.rightMotors->move(throttleCurve->curve(right));
    }
}

void Chassis::arcade(int throttle, int turn, bool disableDriveCurve) {
    // use drive curves if they have not been disabled
    if (!disableDriveCurve) {
        throttle = throttleCurve->curve(throttle);
        turn = throttleCurve->curve(turn);
    }
    int leftPower = throttle + turn;
    int rightPower = throttle - turn;
    // desaturate output
    float max = std::max(std::fabs(leftPower), std::fabs(rightPower)) / 127;
    if (max > 1) {
        leftPower /= max;
        rightPower /= max;
    }
    // move drive
    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
}

void Chassis::curvature(int throttle, int turn, bool disableDriveCurve) {
    // If we're not moving forwards change to arcade drive
    if (throttle == 0) {
        arcade(throttle, turn, disableDriveCurve);
        return;
    }

    // use drive curves if they have not been disabled
    if (!disableDriveCurve) {
        throttle = throttleCurve->curve(throttle);
        turn = throttleCurve->curve(turn);
    }

    float leftPower = throttle + (std::fabs(throttle) * turn / 127.0);
    float rightPower = throttle - (std::fabs(throttle) * turn / 127.0);

    // desaturate output
    float max = std::max(std::fabs(leftPower), std::fabs(rightPower)) / 127;
    if (max > 1) {
        leftPower /= max;
        rightPower /= max;
    }
    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
}
} // namespace lemlib
