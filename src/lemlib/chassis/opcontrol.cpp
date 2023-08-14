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

void Chassis::arcade(int throttle, int turn, float curveGain) {
    int leftPower = calcDriveCurve(throttle + turn, curveGain);
    int rightPower = calcDriveCurve(throttle - turn, curveGain);
    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
};

void Chassis::curvature(int throttle, int curvature, float cureveGain) {
    // If we're not moving forwards change to arcade drive
    if (throttle == 0) {
        arcade(throttle, curvature, cureveGain);
        return;
    }

    double leftPower = throttle + (std::abs(throttle) * curvature) / 127.0;
    double rightPower = throttle - (std::abs(throttle) * curvature) / 127.0;

    leftPower = calcDriveCurve(leftPower, cureveGain);
    rightPower = calcDriveCurve(rightPower, cureveGain);

    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
};
} // namespace lemlib
