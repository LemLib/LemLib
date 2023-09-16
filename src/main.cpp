#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/logger/logger.hpp"

// drive motors
pros::Motor lF(-9, pros::E_MOTOR_GEARSET_06); // left front motor. port 9, reversed
pros::Motor lB(-21, pros::E_MOTOR_GEARSET_06); // left back motor. port 21, reversed
pros::Motor rF(12, pros::E_MOTOR_GEARSET_06); // right front motor. port 12
pros::Motor rB(16, pros::E_MOTOR_GEARSET_06); // right back motor. port 16

// motor groups
pros::MotorGroup leftMotors({lF, lB}); // left motor group
pros::MotorGroup rightMotors({rF, rB}); // right motor group

// Inertial Sensor on port 11
pros::Imu imu(11);

// tracking wheels
pros::Rotation horizontalEnc(7);
// horizontal tracking wheel. 2.75" diameter, 3.7" offset, back of the robot
lemlib::TrackingWheel horizontal(&horizontalEnc, lemlib::Omniwheel::NEW_275, -3.7);

// drivetrain
lemlib::Drivetrain_t drivetrain {&leftMotors, &rightMotors, 10, lemlib::Omniwheel::NEW_325, 360, 2};

// lateral motion controller
lemlib::ChassisController_t lateralController {10, 30, 1, 100, 3, 500, 20};

// angular motion controller
lemlib::ChassisController_t angularController {2, 10, 1, 100, 3, 500, 20};

// sensors for odometry
lemlib::OdomSensors_t sensors {nullptr, nullptr, &horizontal, nullptr, &imu};

lemlib::Chassis chassis(drivetrain, lateralController, angularController, sensors);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    pros::lcd::initialize();
    chassis.calibrate(); // calibrate sensors

    // for more information on how the formatting for the loggers
    // works, refer to the fmtlib docs

    LEMLIB_INFO("{1}, {0}!", "LemLib", "Hello");

    // print odom values to the brain
    pros::Task screenTask([=]() {
        while (true) {
            LEMLIB_DEBUG("Chassis pose: {}", chassis.getPose());
            pros::delay(50);
        }
    });
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() { chassis.moveTo(20, 15, 90, 4000); }
