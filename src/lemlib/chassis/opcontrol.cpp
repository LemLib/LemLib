#include "lemlib/chassis/chassis.hpp"
#include <algorithm>
#include <math.h>

namespace lemlib {

double calcDriveCurve(double input, double scale) {
    if (scale != 0) {
        return (powf(2.718, -(scale / 10)) + powf(2.718, (fabs(input) - 127) / 10) * (1 - powf(2.718, -(scale / 10)))) *
               input;
    }
    return input;
}

void Chassis::tank(int left, int right, float curveGain) {
    drivetrain.leftMotors->move(calcDriveCurve(left, curveGain));
    drivetrain.rightMotors->move(calcDriveCurve(right, curveGain));
};

void Chassis::arcade(int forward, int turn, float curveGain) {
    int leftPower = calcDriveCurve(forward + turn, curveGain);
    int rightPower = calcDriveCurve(forward - turn, curveGain);
    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
};

void Chassis::curvature(int forward, int curvature, float cureveGain) {
    // If we're not moving forwards change to arcade drive
    if (forward == 0) {
        arcade(forward, curvature, cureveGain);
        return;
    }

    double leftPower = forward + (std::abs(forward) * curvature) / 127.0;
    double rightPower = forward - (std::abs(forward) * curvature) / 127.0;

    leftPower = calcDriveCurve(leftPower, cureveGain);
    rightPower = calcDriveCurve(rightPower, cureveGain);

    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
};
} // namespace lemlib
