#include "main.h"
#include "lemlib/api.hpp"

// drive motors
pros::Motor lF(-3, pros::E_MOTOR_GEARSET_06); // left front motor. port 3, reversed
pros::Motor lM(-14, pros::E_MOTOR_GEARSET_06); // left middle motor. port 14, reversed
pros::Motor lB(-12, pros::E_MOTOR_GEARSET_06); // left back motor. port 12, reversed
pros::Motor rF(19, pros::E_MOTOR_GEARSET_06); // right front motor. port 19
pros::Motor rM(20, pros::E_MOTOR_GEARSET_06); // right middle motor. port 20
pros::Motor rB(1, pros::E_MOTOR_GEARSET_06); // right back motor. port 1

// motor groups
pros::MotorGroup leftMotors({lF, lM, lB}); // left motor group
pros::MotorGroup rightMotors({rF, rM, rB}); // right motor group

// Inertial Sensor on port 6
pros::Imu imu(6);

// tracking wheels
pros::ADIEncoder verticalEnc('A', 'B', false);
// vertical tracking wheel. 2.75" diameter, 2.2" offset
lemlib::TrackingWheel vertical(&verticalEnc, lemlib::Omniwheel::NEW_275, 2.2);

// drivetrain
lemlib::Drivetrain_t drivetrain {
    &leftMotors, &rightMotors, 10, lemlib::Omniwheel::NEW_325, 360,
};

// lateral motion controller
lemlib::ChassisController_t lateralController {10, 30, 1, 100, 3, 500, 20};

// angular motion controller
lemlib::ChassisController_t angularController {2, 10, 1, 100, 3, 500, 3};

// sensors for odometry
lemlib::OdomSensors_t sensors {nullptr, nullptr, nullptr, nullptr, &imu};

lemlib::Chassis chassis(drivetrain, lateralController, angularController, sensors);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    pros::lcd::initialize();
    // calibrate sensors
    chassis.calibrate();
    // print odom values to the brain
    while (true) {
        pros::lcd::print(0, "X: %f", chassis.getPose().x);
        pros::lcd::print(1, "Y: %f", chassis.getPose().y);
        pros::lcd::print(2, "Theta: %f", chassis.getPose().theta);
        pros::delay(10);
    }
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
void autonomous() {
    // move the chassis to x: 20, y: 0, facing heading: 90
    chassis.moveTo(20, 0, 90, 4000);
}

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
void opcontrol() {}
