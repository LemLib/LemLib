#include "main.h"
#include "lemlib/api.hpp"

pros::Motor lF(-1);
pros::Motor lM(-3);
pros::Motor lB(-11);
pros::Motor rF(10);
pros::Motor rM(4);
pros::Motor rB(5);

pros::MotorGroup leftMotors({lF, lM, lB});
pros::MotorGroup rightMotors({rF, rM, rB});

pros::Imu imu(8);

pros::ADIEncoder verticalEnc('e', 'f');
pros::ADIEncoder horizontalEnc('g', 'h');

pros::Controller master(pros::E_CONTROLLER_MASTER);

lemlib::TrackingWheel vertical(&verticalEnc, 2.75, 0);
lemlib::TrackingWheel horizontal(&horizontalEnc, 2.75, -2.496);

lemlib::OdomSensors_t sensors {
	&vertical,
	nullptr,
	&horizontal,
	nullptr,
	&imu
};

lemlib::ChassisController_t lateralController {
	0,
	10,
	30,
	1,
	100000000,
	3,
	500000000
};

lemlib::ChassisController_t angularController {
	0,
	2,
	10,
	1,
	100000000,
	3,
	5000000000
};

lemlib::Chassis chassis(&leftMotors, &rightMotors, 10000, lateralController, angularController, sensors);



/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
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
	chassis.calibrate();
	chassis.setPose(0, 0, 180);
	chassis.moveTo(0, 30, 1000000);
    
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
void opcontrol() {
	
}
