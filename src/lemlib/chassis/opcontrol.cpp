#include "lemlib/chassis/chassis.hpp"
#include "lemlib/util.hpp"
#include <math.h>

namespace lemlib {

/**
 * @brief These settings are used to optimize drivetrain control during Operator control.
 *
 * https://www.desmos.com/calculator/umicbymbnl
 *
 * @param deadband range where inputs will be ignored
 * @param minOutput minimum output required to make the drivetrain move
 * @param curve how curved the graph is. Set to 0 for linear
 */
OpcontrolSettings::OpcontrolSettings(float deadband, float minOutput, float curve, DriveCurveFunction_t driveCurve)
    : deadband(deadband),
      minOutput(minOutput),
      curve(curve),
      driveCurve(driveCurve) {}

OpcontrolSettings defaultOpcontrolSettings = OpcontrolSettings(0, 0, 0);

/**
 * @brief Exponential drive curve. Allows for fine control at low speeds while maintaining the same maximum speed.
 *
 * Interactive Graph: https://www.desmos.com/calculator/umicbymbnl
 *
 * @param input value from -127 to 127
 * @param inputDeadband range where inputs will be ignored (outputs 0), which can be optionally ignored
 * @param minOutput the minimum output required to make the drivetrain move, which can be optionally ignored
 * @param curve how steep the curve should be.
 * @return The new value to be used.
 */
float expoDriveCurve(float input, float inputDeadband, float minOutput, float curve) {
    if (fabs(input) < inputDeadband) return 0;
    if (curve < 1) curve = 1;
    // g is the output of g(x) as defined in the Desmos graph
    const float g = fabs(input) - inputDeadband;
    // g127 is the output of g(127) as defined in the Desmos graph
    const float g127 = 127 - inputDeadband;
    // i is the output of i(x) as defined in the Desmos graph
    const float i = pow(curve, g - 127) * g * sgn(input);
    // i127 is the output of i(127) as defined in the Desmos graph
    const float i127 = pow(curve, g127 - 127) * g127;
    return (127.0 - minOutput) / (127) * i * 127 / i127 + minOutput * sgn(input);
}

/**
 * @brief Control the robot during the driver using the arcade drive control scheme. In this control scheme one
 * joystick axis controls the forwards and backwards movement of the robot, while the other joystick axis
 * controls  the robot's turning
 * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
 * @param turn speed to turn. Takes an input from -127 to 127.
 */
void Chassis::tank(int left, int right) {
    drivetrain.leftMotors->move(opcontrolSettings.driveCurve(left, opcontrolSettings.deadband,
                                                             opcontrolSettings.minOutput, opcontrolSettings.curve));
    drivetrain.rightMotors->move(opcontrolSettings.driveCurve(right, opcontrolSettings.deadband,
                                                              opcontrolSettings.minOutput, opcontrolSettings.curve));
}

/**
 * @brief Control the robot during the driver using the arcade drive control scheme. In this control scheme one
 * joystick axis controls the forwards and backwards movement of the robot, while the other joystick axis
 * controls the robot's turning
 * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
 * @param turn speed to turn. Takes an input from -127 to 127.
 */
void Chassis::arcade(int throttle, int turn) {
    int leftPowerRaw = opcontrolSettings.driveCurve(throttle + turn, opcontrolSettings.deadband,
                                                    opcontrolSettings.minOutput, opcontrolSettings.curve);
    int rightPowerRaw = opcontrolSettings.driveCurve(throttle - turn, opcontrolSettings.deadband,
                                                     opcontrolSettings.minOutput, opcontrolSettings.curve);
    // desaturate output
    int leftPower = leftPowerRaw / (fabs(leftPowerRaw) + fabs(rightPowerRaw)) * 127;
    int rightPower = rightPowerRaw / (fabs(leftPowerRaw) + fabs(rightPowerRaw)) * 127;
    // move drive
    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
}

/**
 * @brief Control the robot during the driver using the curvature drive control scheme. This control scheme is
 * very similar to arcade drive, except the second joystick axis controls the radius of the curve that the
 * drivetrain makes, rather than the speed. This means that the driver can accelerate in a turn without changing
 * the radius of that turn. This control scheme defaults to arcade when forward is zero.
 * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
 * @param turn speed to turn. Takes an input from -127 to 127.
 */
void Chassis::curvature(int throttle, int turn) {
    // If we're not moving forwards change to arcade drive
    if (throttle == 0) {
        arcade(throttle, turn);
        return;
    }

    float leftPower = throttle + (std::abs(throttle) * turn) / 127.0;
    float rightPower = throttle - (std::abs(throttle) * turn) / 127.0;

    leftPower = opcontrolSettings.driveCurve(leftPower, opcontrolSettings.deadband, opcontrolSettings.minOutput,
                                             opcontrolSettings.curve);
    rightPower = opcontrolSettings.driveCurve(rightPower, opcontrolSettings.deadband, opcontrolSettings.minOutput,
                                              opcontrolSettings.curve);

    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
}
} // namespace lemlib
