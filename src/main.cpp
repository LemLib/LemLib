#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/logger/stdout.hpp"
#include "pros/misc.h"

// controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// drive motors
pros::Motor lF(-8, pros::E_MOTOR_GEARSET_06); // left front motor. port 8, reversed
pros::Motor lM(-20, pros::E_MOTOR_GEARSET_06); // left middle motor. port 20, reversed
pros::Motor lB(19, pros::E_MOTOR_GEARSET_06); // left back motor. port 19
pros::Motor rF(2, pros::E_MOTOR_GEARSET_06); // right front motor. port 2
pros::Motor rM(11, pros::E_MOTOR_GEARSET_06); // right middle motor. port 11
pros::Motor rB(-13, pros::E_MOTOR_GEARSET_06); // right back motor. port 13, reversed

// motor groups
pros::MotorGroup leftMotors({lF, lM, lB}); // left motor group
pros::MotorGroup rightMotors({rF, rM, rB}); // right motor group

// Inertial Sensor on port 11
pros::Imu imu(12);

// tracking wheels
pros::Rotation horizontalEnc(4);
// horizontal tracking wheel. 2.75" diameter, 3.7" offset, back of the robot
lemlib::TrackingWheel horizontal(&horizontalEnc, lemlib::Omniwheel::NEW_275, -3.7);

// drivetrain
lemlib::Drivetrain_t drivetrain {&leftMotors, &rightMotors, 10, lemlib::Omniwheel::NEW_4, 300, 8};

// lateral motion controller
lemlib::ChassisController_t lateralController {
    10, // proportional gain (kP)
    30, // derivative gain (kD)
    1, // small error range, in inches
    100, // small error range timeout, in milliseconds
    3, // large error range, in inches
    500, // large error range timeout, in milliseconds
    20 // maximum acceleration (slew)
};

// angular motion controller
lemlib::ChassisController_t angularController {
    2, // proportional gain (kP)
    10, // derivative gain (kD)
    1, // small error range, in degrees
    100, // small error range timeout, in milliseconds
    3, // large error range, in degrees
    500, // large error range timeout, in milliseconds
    20 // maximum acceleration (slew)
};

// sensors for odometry
lemlib::OdomSensors_t sensors {&vertical, nullptr, nullptr, nullptr, &imu};

// create the chassis
lemlib::Chassis chassis(drivetrain, lateralController, angularController, sensors);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    pros::lcd::initialize(); // initialize brain screen
    chassis.calibrate(); // calibrate sensors

    // the default rate is 50. however, if you need to change the rate, you
    // can do the following.
    // lemlib::bufferedStdout().setRate(...);
    // If you use bluetooth or a wired connection, you will want to have a rate of 10ms

    // for more information on how the formatting for the loggers
    // works, refer to the fmtlib docs
    std::cout << std::setprecision(5);

    // thread to for brain screen and position logging
    pros::Task screenTask([&]() {
        lemlib::Pose pose(0, 0, 0);
        while (true) {
            // print robot location to the brain screen
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            // log position telemetry
            lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());
            // delay to save resources
            pros::delay(50);
        }
    });
}

/**
 * Runs while the robot is disabled
 */
void disabled() {}

/**
 * runs after initialize if the robot is connected to field control
 */
void competition_initialize() {}

// get a path used for pure pursuit
// this needs to be put outside a function
ASSET(example_txt); // '.' replaced with "_" to make c++ happy

/**
 * Runs during auto
 *
 * This is an example autonomous routine which demonstrates a lot of the features LemLib has to offer
 */
void opcontrol() { chassis.moveToOld(20, 15, 3000, false); }
