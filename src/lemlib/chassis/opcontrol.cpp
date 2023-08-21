#include "lemlib/chassis/chassis.hpp"
#include <algorithm>
#include <math.h>

namespace lemlib {

/**
 * @brief  Default drive curve. If the input is 127, the function will always output 127, no
 * matter the value of scale, likewise for -127. This curve was inspired by team 5225, the Pilons. A Desmos graph of
 * this curve can be found here: https://www.desmos.com/calculator/rcfjjg83zx
 * @param input value from -127 to 127
 * @param scale how steep the curve should be.
 */
double defaultDriveCurve(double input, double scale) {
    if (scale != 0) {
        return (powf(2.718, -(scale / 10)) + powf(2.718, (fabs(input) - 127) / 10) * (1 - powf(2.718, -(scale / 10)))) *
               input;
    }
    return input;
}

/**
 * @brief Control the robot during the driver using the arcade drive control scheme. In this control scheme one
 * joystick axis controls the forwards and backwards movement of the robot, while the other joystick axis
 * controls  the robot's turning
 * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
 * @param turn speed to turn. Takes an input from -127 to 127.
 * @param curveGain control how steep the drive curve is. The larger the number, the steeper the curve. A value
 * of 1 disables the curve entirely.
 */
void Chassis::tank(int left, int right, float curveGain) {
    drivetrain.leftMotors->move(driveCurve(left, curveGain));
    drivetrain.rightMotors->move(driveCurve(right, curveGain));
};

/**
 * @brief Control the robot during the driver using the arcade drive control scheme. In this control scheme one
 * joystick axis controls the forwards and backwards movement of the robot, while the other joystick axis
 * controls  the robot's turning
 * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
 * @param turn speed to turn. Takes an input from -127 to 127.
 * @param curveGain control how steep the drive curve is. The larger the number, the steeper the curve. A value
 * of 0 disables the curve entirely.
 */
void Chassis::arcade(int throttle, int turn, float curveGain) {
    int leftPower = driveCurve(throttle + turn, curveGain);
    int rightPower = driveCurve(throttle - turn, curveGain);
    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
};

/**
 * @brief Control the robot during the driver using the curvature drive control scheme. This control scheme is
 * very similar to arcade drive, except the second joystick axis controls the radius of the curve that the
 * drivetrain makes, rather than the speed. This means that the driver can accelerate in a turn without changing
 * the radius of that turn. This control scheme defaults to arcade when forward is zero.
 * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
 * @param turn speed to turn. Takes an input from -127 to 127.
 * @param curveGain control how steep the drive curve is. The larger the number, the steeper the curve. A value
 * of 0 disables the curve entirely.
 */
void Chassis::curvature(int throttle, int turn, float curveGain) {
    // If we're not moving forwards change to arcade drive
    if (throttle == 0) {
        arcade(throttle, turn, curveGain);
        return;
    }

    double leftPower = throttle + (std::abs(throttle) * turn) / 127.0;
    double rightPower = throttle - (std::abs(throttle) * turn) / 127.0;

    leftPower = driveCurve(leftPower, curveGain);
    rightPower = driveCurve(rightPower, curveGain);

    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
};
} // namespace lemlib
