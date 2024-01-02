#include "lemlib/chassis/chassis.hpp"
#include <algorithm>
#include <math.h>

namespace lemlib {

/**
 * @brief  Default drive curve. Modifies the input with an exponential curve. If the input is 127, the function
 * will always output 127, no matter the value of scale, likewise for -127. A scale of zero disable the curve
 * entirely. This curve was inspired by team 5225, the Pilons. A Desmos graph of this curve can be found
 * here: https://www.desmos.com/calculator/rcfjjg83zx
 * @param input value from -127 to 127
 * @param scale how steep the curve should be.
 * @return The new value to be used.
 */
float defaultDriveCurve(float input, float scale) {
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
 * @param curveGain the scale inputted into the drive curve function. If you are using the default drive
 * curve, refer to the `defaultDriveCurve` documentation.
 */
void Chassis::tank(int left, int right, float leftCurveGain, float rightCurveGain,
                   const DriveCurveFunction_t& leftCurve, const DriveCurveFunction_t& rightCurve) {
    drivetrain.leftMotors->move(leftCurve(left, leftCurveGain));
    drivetrain.rightMotors->move(rightCurve(right, rightCurveGain));
}

/**
 * @brief Control the robot during the driver using the arcade drive control scheme. In this control scheme one
 * joystick axis controls the forwards and backwards movement of the robot, while the other joystick axis
 * controls  the robot's turning
 * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
 * @param turn speed to turn. Takes an input from -127 to 127.
 * @param curveGain the scale inputted into the drive curve function. If you are using the default drive
 * curve, refer to the `defaultDriveCurve` documentation.
 */
void Chassis::arcade(int throttle, int turn, float linearCurveGain, float turnCurveGain,
                     const DriveCurveFunction_t& driveCurve, const DriveCurveFunction_t& turnCurve) {
    int leftPower = driveCurve(throttle, linearCurveGain) + turnCurve(turn, linearCurveGain);
    int rightPower = driveCurve(throttle, linearCurveGain) - turnCurve(turn, linearCurveGain);
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
 * @param curveGain the scale inputted into the drive curve function. If you are using the default drive
 * curve, refer to the `defaultDriveCurve` documentation.
 */
void Chassis::curvature(int throttle, int turn, float linearCurveGain, float turnCurveGain,
                        const DriveCurveFunction_t& driveCurve, const DriveCurveFunction_t& turnCurve) {
    // If we're not moving forwards change to arcade drive
    if (throttle == 0) {
        this->arcade(throttle, turn, linearCurveGain, turnCurveGain, driveCurve, turnCurve);
        return;
    }

    float curvedThrottle = driveCurve(throttle, linearCurveGain);
    float curvedTurn = turnCurve(turn, turnCurveGain);
    float leftPower = curvedThrottle + (std::abs(curvedThrottle) * curvedTurn) / 127.0;
    float rightPower = curvedThrottle - (std::abs(curvedThrottle) * curvedTurn) / 127.0;

    leftPower = driveCurve(leftPower, linearCurveGain);
    rightPower = turnCurve(rightPower, turnCurveGain);

    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
}
} // namespace lemlib
