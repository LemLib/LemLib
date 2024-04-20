#include "lemlib/chassis/chassis.hpp"
#include "lemlib/util.hpp"
#include <math.h>

namespace lemlib {

ExpoDriveCurve defaultDriveCurve = ExpoDriveCurve(0, 0, 1);

/**
 * @brief Control the robot during the driver using the arcade drive control scheme. In this control scheme one
 * joystick axis controls the forwards and backwards movement of the robot, while the other joystick axis
 * controls  the robot's turning
 *
 * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
 * @param turn speed to turn. Takes an input from -127 to 127.
 * @param disableDriveCurve whether to disable the drive curve or not. If disabled, uses a linear curve with no
 * deadzone or minimum power
 */
void Chassis::tank(int left, int right, bool disableDriveCurve) {
    if (disableDriveCurve) {
        drivetrain.leftMotors->move(left);
        drivetrain.rightMotors->move(right);
    } else {
        drivetrain.leftMotors->move(throttleCurve->curve(left));
        drivetrain.rightMotors->move(throttleCurve->curve(right));
    }
}

/**
 * @brief Control the robot during the driver using the arcade drive control scheme. In this control scheme one
 * joystick axis controls the forwards and backwards movement of the robot, while the other joystick axis
 * controls the robot's turning
 *
 * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
 * @param turn speed to turn. Takes an input from -127 to 127.
 * @param disableDriveCurve whether to disable the drive curve or not. If disabled, uses a linear curve with no
 * deadzone or minimum power
 * @param desaturateBias how much to favor turning over forward/backward motion or vice versa when motors are
 * saturated. A value of 0 fully prioritizes driving, a value of 1 fully prioritizes turning
 */
void Chassis::arcade(int throttle, int turn, bool disableDriveCurve, float desaturateBias) {
    // use drive curves if they have not been disabled
    if (!disableDriveCurve) {
        throttle = throttleCurve->curve(throttle);
        turn = throttleCurve->curve(turn);
    }
    // desaturate motors based on joyBias
    {
        int oldThrottle = throttle;
        int oldTurn = turn;
        throttle *= (1 - desaturateBias * std::abs(oldTurn / 127));
        turn *= (1 - (1 - desaturateBias) * std::abs(oldThrottle / 127));
    }

    int leftPower = throttle + turn;
    int rightPower = throttle - turn;

    // move drive
    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
}

/**
 * @brief Control the robot during the driver using the curvature drive control scheme. This control scheme is
 * very similar to arcade drive, except the second joystick axis controls the radius of the curve that the
 * drivetrain makes, rather than the speed. This means that the driver can accelerate in a turn without changing
 * the radius of that turn. This control scheme defaults to arcade when forward is zero.
 *
 * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
 * @param turn speed to turn. Takes an input from -127 to 127.
 * @param disableDriveCurve whether to disable the drive curve or not. If disabled, uses a linear curve with no
 * deadzone or minimum power
 */
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
