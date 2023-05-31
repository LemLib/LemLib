#include "main.h"
#include "lemlib/api.hpp"
using namespace lemlib;

// drive motors
pros::Motor lF(-3,
               pros::E_MOTOR_GEARSET_06); // left front motor. port 3, reversed
pros::Motor lM(-14, pros::E_MOTOR_GEARSET_06); // left middle motor. port 14, reversed
pros::Motor lB(-12,
               pros::E_MOTOR_GEARSET_06); // left back motor. port 12, reversed
pros::Motor rF(19, pros::E_MOTOR_GEARSET_06); // right front motor. port 19
pros::Motor rM(20, pros::E_MOTOR_GEARSET_06); // right middle motor. port 20
pros::Motor rB(1, pros::E_MOTOR_GEARSET_06); // right back motor. port 1
pros::Motor dirControl(1, pros::E_MOTOR_GEARSET_06);
// motor groups
pros::MotorGroup leftMotors({lF, lM, lB}); // left motor group
pros::MotorGroup rightMotors({rF, rM, rB}); // right motor group

// Inertial Sensor on port 6
pros::Imu imu(6);

// tracking wheels
pros::ADIEncoder verticalEnc('A', 'B', false);
// vertical tracking wheel. 2.75" diameter, 2.2" offset
lemlib::TrackingWheel vertical(&verticalEnc, 2.75, 0);

// drivetrain
lemlib::DifferentialDrive::Drivetrain_t drivetrain {
    &leftMotors, &rightMotors, 10, 3.25, 360,
};

lemlib::XDrive::Drivetrain_t holoDrivetrain {
    &rF, &rB, &lF, &lB, 10, 3.25, 360,
};

// lateral motion controller
lemlib::DifferentialDrive::ChassisController_t lateralController {10, 30, 1, 100, 3, 500, 20};

// angular motion controller
lemlib::DifferentialDrive::ChassisController_t angularController {2, 10, 1, 100, 3, 500, 3};

// sensors for odometry
lemlib::DifferentialDrive::OdomSensors_t sensors {nullptr, nullptr, nullptr, nullptr, &imu};

lemlib::XDrive::ChassisController_t strafeController {2, 10, 1, 100, 3, 500, 3};

lemlib::SwerveDrive<lemlib::SwerveType_t::Coaxial>::Drivetrain_t swerveSetting {&lF, &lB, &rF, &rB,  &dirControl, &lF,
                                                                                &lB, &rF, 10,  3.25, 360};

auto diffDrive = lemlib::ChassisBuilder<lemlib::DifferentialDrive>()
                     .withSetting(drivetrain)
                     .withLateralController(lateralController)
                     .withAngularController(angularController)
                     .withSensor(sensors)
                     .build();

auto holoDrive = lemlib::ChassisBuilder<lemlib::XDrive>() // Holonomic = XDrive = MecanumDrive
                     .withSetting(holoDrivetrain)
                     .withLateralController(lateralController)
                     .withAngularController(angularController)
                     .withStrafeController(strafeController)
                     .withSensor(sensors)
                     .build();

using namespace lemlib;

auto bruhDrive = ChassisBuilder<SwerveDrive<SwerveType_t::Coaxial>>()
                     .withSetting(swerveSetting)
                     .withLateralController(lateralController)
                     .withAngularController(angularController)
                     .withStrafeController(strafeController)
                     .withSensor(sensors)
                     .build();

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    pros::lcd::initialize();
    // calibrate sensors
    while (true) {}
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
void opcontrol() {}
