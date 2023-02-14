#include "main.h"
#include "lemlib/api.hpp"
#include "pros/misc.h"

// drive motors
pros::Motor lF(-3); // left front motor. port 3, reversed
pros::Motor lM(-14); // left middle motor. port 14, reversed
pros::Motor lB(-12); // left back motor. port 12, reversed
pros::Motor rF(19); // right front motor. port 19
pros::Motor rM(20); // right middle motor. port 20
pros::Motor rB(1); // right back motor. port 1

// intake
pros::Motor intake(2); // intake motor. port 2

// motor groups
pros::MotorGroup leftMotors({lF, lM, lB}); // left motor group
pros::MotorGroup rightMotors({rF, rM, rB}); // right motor group

// Inertial Sensor on port 6
pros::Imu imu(6);

// tracking wheels
pros::ADIEncoder verticalEnc({7, 'A', 'B'}, false);
// vertical tracking wheel. 2.75" diameter, 2.2" offset
lemlib::TrackingWheel vertical(&verticalEnc, 2.75, 2.2);

lemlib::OdomSensors_t sensors {
	&vertical,
	nullptr,
	nullptr,
	nullptr,
	&imu
};

lemlib::ChassisController_t lateralController {
	10,
	30,
	1,
	100,
	3,
	500
};

lemlib::ChassisController_t angularController {
	2,
	10,
	1,
	100,
	3,
	500
};

lemlib::Chassis chassis(&leftMotors, &rightMotors, 10, lateralController, angularController, sensors);



/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	// calibrate sensors
	chassis.calibrate();
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
	// set the position of the chassis
	chassis.setPose(10, 10, 90);
	// turn to face point (53, 53) and move to it
	chassis.turnTo(53, 53, 1000);
	// move to point (20, 20)
	chassis.moveTo(20, 20, 2000);
	// use pure pursuit to follow a path
	chassis.follow("path.txt", 1000, 15);
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
	// create a new macro manager
	lemlib::macro::MacroManager macros;

	// create a new macro
	// it runs the intake when the Controller L2 button is pressed, and stops it when it is released
	lemlib::macro::Macro intakeMacro = lemlib::macro::Macro(
		{pros::E_CONTROLLER_DIGITAL_L1}, // the sequence of buttons to press, in this case, only L2
		[]() { intake.move_velocity(200); }, // the function to run when the sequence is pressed (starts the intake)
		[]() { if (!intake.is_stopped()) intake.move_velocity(0); } // the function to run when the sequence is released (stops the intake)
	);

	// add the macro to the macro manager
	macros.addMacro(intakeMacro);

	// the main controller
	pros::Controller controller(pros::E_CONTROLLER_MASTER);

	while (true) {
		// update the macro manager
		macros.check(controller);

		// delay the task to prevent hogging the CPU
		// most devices poll at 10ms intervals, so this is a good value to use
		pros::delay(10);
	}
}
