/**
 * @file src/lemlib/chassis/chassis.cpp
 * @author LemLib Team
 * @brief definitions for the chassis class
 * @version 0.4.5
 * @date 2023-01-27
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "lemlib/chassis/differential.hpp"
#include "lemlib/util.hpp"
#include "lemlib/pid.hpp"
#include "lemlib/movements/boomerang.hpp"
#include "lemlib/movements/purepursuit.hpp"
#include "lemlib/movements/turn.hpp"
#include "lemlib/odom/differentialArc.hpp"
#include "lemlib/devices/gyro/imu.hpp"

namespace lemlib {
std::shared_ptr<pros::MotorGroup> makeMotorGroup(const std::initializer_list<int8_t> ports,
                                                 const pros::v5::MotorGears gears) {
    // create the shared pointer
    std::shared_ptr<pros::MotorGroup> motors =
        std::make_shared<pros::MotorGroup>(std::initializer_list<int8_t> {ports}, gears);
    return motors;
}

/**
 * Construct a new Chassis
 *
 * Most member variables get initialized here, in an initializer list.
 * A notable exception is the odometry, which at the moment is too complex to
 * construct in the initializer list
 */
Differential::Differential(Drivetrain drivetrain, ControllerSettings linearSettings, ControllerSettings angularSettings,
                           OdomSensors sensors)
    : drivetrain(drivetrain),
      linearSettings(linearSettings),
      angularSettings(angularSettings) {
    // create sensor vectors
    std::vector<TrackingWheel> verticals;
    std::vector<TrackingWheel> horizontals;
    std::vector<TrackingWheel> drive;
    std::vector<std::shared_ptr<Gyro>> imus;

    // configure vertical tracking wheels
    if (sensors.vertical1 != nullptr) verticals.push_back(*sensors.vertical1); // add vertical tracker if configured
    if (sensors.vertical2 != nullptr) verticals.push_back(*sensors.vertical2); // add vertical tracker if configured

    // configure horizontal tracking wheels
    if (sensors.horizontal1 != nullptr) horizontals.push_back(*sensors.horizontal1);
    if (sensors.horizontal2 != nullptr) horizontals.push_back(*sensors.horizontal2);

    // configure drivetrain
    drive.push_back(
        TrackingWheel(drivetrain.leftMotors, drivetrain.wheelDiameter, -drivetrain.trackWidth / 2, drivetrain.rpm));
    drive.push_back(
        TrackingWheel(drivetrain.leftMotors, drivetrain.wheelDiameter, drivetrain.trackWidth / 2, drivetrain.rpm));

    // configure imu
    if (sensors.imu != nullptr) imus.push_back(std::make_shared<Imu>(*sensors.imu));

    // create odom instance
    odom = std::make_unique<DifferentialArc>(DifferentialArc(verticals, horizontals, drive, imus));
}

/**
 * Initialize the chassis
 *
 * Calibrates sensors and starts the chassis task
 */
void Differential::initialize() {
    // calibrate odom
    odom->calibrate();
    // start the chassis task if it doesn't exist
    if (task == nullptr)
        task = std::make_unique<pros::Task>([&]() {
            while (true) {
                update();
                pros::delay(10);
            }
        });
}

/**
 * This function sets up the Turn controller
 *
 * Like all chassis movement functions, it sets a member pointer to a new movement.
 * the movement is a derived class of the Movement class
 *
 * There are some things that need to be done before instantiating the movement however.
 * It needs to set up a PID which the movement will use to turn the robot. We also need
 * to convert the x and y values given passed in to a Pose object. All that needs to be
 * done then is to pass the parameters to a new instance of Turn, and set the movement
 * pointer.
 */
void Differential::turnToPose(float x, float y, int timeout, bool reversed, int maxSpeed) {
    // if a movement is already running, wait until it is done
    if (movement != nullptr) waitUntilDone();
    // set up the PID
    FAPID angularPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    angularPID.setExit(angularSettings.largeError, angularSettings.smallError, angularSettings.largeErrorTimeout,
                       angularSettings.smallErrorTimeout, timeout);
    // create the movement
    movement = std::make_unique<Turn>(angularPID, Pose(x, y), reversed, maxSpeed);
}

/**
 * This function sets up the Turn controller
 *
 * Like all chassis movement functions, it sets a member pointer to a new movement.
 * the movement is a derived class of the Movement class
 *
 * There are some things that need to be done before instantiating the movement however.
 * It needs to set up a PID which the movement will use to turn the robot. We also need to
 * convert the heading passed by the user to radians and standard position. All that needs to be
 * done then is to pass the parameters to a new instance of Turn, and set the movement
 * pointer.
 */
void Differential::turnToHeading(float heading, int timeout, int maxSpeed) {
    // if a movement is already running, wait until it is done
    if (movement != nullptr) waitUntilDone();
    // convert heading to radians and standard form
    float newHeading = M_PI_2 - degToRad(heading);
    // set up the PID
    FAPID angularPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    angularPID.setExit(angularSettings.largeError, angularSettings.smallError, angularSettings.largeErrorTimeout,
                       angularSettings.smallErrorTimeout, timeout);
    // create the movement
    movement = std::make_unique<Turn>(angularPID, newHeading, maxSpeed);
}

/**
 * This function sets up the Boomerang controller
 *
 * Like all chassis movement functions, it sets a member pointer to a new movement.
 * the movement is a derived class of the Movement class.
 *
 * There are some things that need to be done before instantiating the movement however.
 * Two PIDs need to be set up to be passed to the Boomerang constructor, and the target heading
 * needs to be converted to radians and standard form.
 * It also needs to decide what the chasePower should be. Usually this will be the value set in
 * the drivetrain struct, but it can be overridden by the user if needed.
 */
void Differential::moveTo(float x, float y, float theta, int timeout, bool forwards, float chasePower, float lead,
                          int maxSpeed) {
    // if a movement is already running, wait until it is done
    if (movement != nullptr) waitUntilDone();
    // convert target theta to radians and standard form
    Pose target = Pose(x, y, M_PI_2 - degToRad(theta));
    // set up PIDs
    FAPID linearPID(0, 0, linearSettings.kP, 0, linearSettings.kD, "linearPID");
    linearPID.setExit(linearSettings.largeError, linearSettings.smallError, linearSettings.largeErrorTimeout,
                      linearSettings.smallErrorTimeout, timeout);
    FAPID angularPID(0, 0, angularSettings.kP, 0, angularSettings.kD, "angularPID");
    // if chasePower is 0, is the value defined in the drivetrain struct
    if (chasePower == 0) chasePower = drivetrain.chasePower;
    // create the movement
    movement = std::make_unique<Boomerang>(linearPID, angularPID, target, forwards, chasePower, lead, maxSpeed);
}

/**
 * This function sets up Pure Pursuit
 *
 * Unlike the Differential::moveTo function, we can just pass the parameters directly to the
 * Pure Pursuit constructor
 */
void Differential::follow(const asset& path, float lookahead, int timeout, bool forwards, int maxSpeed) {
    // if a movement is already running, wait until it is done
    if (movement != nullptr) waitUntilDone();
    // create the movement
    movement = std::make_unique<PurePursuit>(drivetrain.trackWidth, path, lookahead, timeout, forwards, maxSpeed);
}

/**
 * Chassis update function
 *
 * This function is called in a loop by the chassis task
 * It updates any motion controller that may be running
 * And it updates the odometry
 * Once implemented, it will also update the drivetrain velocity controllers
 */
void Differential::update() {
    // update odometry
    odom->update();
    // update the motion controller, if one is running
    if (movement != nullptr) {
        std::pair<int, int> output = movement->update(odom->getPose()); // get output
        if (output.first == 128 && output.second == 128) { // if the movement is done
            movement = nullptr; // stop movement
            output.first = 0;
            output.second = 0;
        }
        // move the motors
        drivetrain.leftMotors->move(output.first);
        drivetrain.rightMotors->move(output.second);
    }
}

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
 * @brief Control the robot during the driver using the curvature drive control scheme. This control scheme is
 * very similar to arcade drive, except the second joystick axis controls the radius of the curve that the
 * drivetrain makes, rather than the speed. This means that the driver can accelerate in a turn without changing
 * the radius of that turn. This control scheme defaults to arcade when forward is zero.
 * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
 * @param turn speed to turn. Takes an input from -127 to 127.
 * @param curveGain the scale inputted into the drive curve function. If you are using the default drive
 * curve, refer to the `defaultDriveCurve` documentation.
 */
void Differential::curvature(int throttle, int turn, float curveGain, DriveCurveFunction_t driveCurve) {
    // If we're not moving forwards change to arcade drive
    if (throttle == 0) {
        arcade(throttle, turn, curveGain);
        return;
    }

    float leftPower = throttle + (std::abs(throttle) * turn) / 127.0;
    float rightPower = throttle - (std::abs(throttle) * turn) / 127.0;

    leftPower = driveCurve(leftPower, curveGain);
    rightPower = driveCurve(rightPower, curveGain);

    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
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
void Differential::arcade(int throttle, int turn, float curveGain, DriveCurveFunction_t driveCurve) {
    int leftPower = driveCurve(throttle + turn, curveGain);
    int rightPower = driveCurve(throttle - turn, curveGain);
    drivetrain.leftMotors->move(leftPower);
    drivetrain.rightMotors->move(rightPower);
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
void Differential::tank(int left, int right, float curveGain, DriveCurveFunction_t driveCurve) {
    drivetrain.leftMotors->move(driveCurve(left, curveGain));
    drivetrain.rightMotors->move(driveCurve(right, curveGain));
}
}; // namespace lemlib