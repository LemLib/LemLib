#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/devices/motor/abstractmotor.hpp"
#include "lemlib/devices/motor/prosmotor.hpp"
#include "lemlib/logger/stdout.hpp"
#include "lemlib/devices/motor/prosmotorgroup.hpp"
#include "pros/abstract_motor.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include <memory>

// controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);

lemlib::MotorInfo leftFrontInfo(1, true, 1, 600);
lemlib::MotorInfo leftMidInfo(2, true, 1, 600);
lemlib::MotorInfo leftBackInfo(3, true, 1, 600);
lemlib::MotorInfo leftStackInfo(4, true, 1, 600);

lemlib::MotorInfo rightFrontInfo(5, true, 1, 600);
lemlib::MotorInfo rightMidInfo(6, true, 1, 600);
lemlib::MotorInfo rightBackInfo(7, true, 1, 600);
lemlib::MotorInfo rightStackInfo(8, true, 1, 600);

lemlib::PROSMotorGroup
    leftMotors({leftFrontInfo, leftMidInfo, leftBackInfo, leftStackInfo});
lemlib::PROSMotorGroup
    rightMotors({rightBackInfo, rightMidInfo, rightFrontInfo, rightStackInfo});

pros::Imu imu(11);

// horizontal tracking wheel. Port 4, 2.75" diameter, 3.7" offset, back of the robot
lemlib::TrackingWheel horizontal(4, lemlib::Omniwheel::NEW_275, -3.7);

// drivetrain settings
lemlib::Drivetrain drivetrain(std::make_shared<lemlib::PROSMotorGroup>(std::move(leftMotors)), // left motor group
                              std::make_shared<lemlib::PROSMotorGroup>(std::move(rightMotors)), // right motor group
                              10, // 10 inch track width
                              lemlib::Omniwheel::NEW_325, // using new 3.25" omnis
                              450, // drivetrain rpm is 360
                              2 // chase power is 2. If we had traction wheels, it would have been 8
);

// linear motion controller
lemlib::ControllerSettings linearController(10, // proportional gain (kP)
                                            30, // derivative gain (kD)
                                            1, // small error range, in inches
                                            100, // small error range timeout, in milliseconds
                                            3, // large error range, in inches
                                            500, // large error range timeout, in milliseconds
                                            20 // maximum acceleration (slew)
);

// angular motion controller
lemlib::ControllerSettings angularController(2, // proportional gain (kP)
                                             10, // derivative gain (kD)
                                             1, // small error range, in degrees
                                             100, // small error range timeout, in milliseconds
                                             3, // large error range, in degrees
                                             500, // large error range timeout, in milliseconds
                                             20 // maximum acceleration (slew)
);

// sensors for odometry
// note that in this example we use internal motor encoders, so we don't pass vertical tracking wheels
lemlib::OdomSensors sensors(nullptr, // vertical tracking wheel 1, set to nullptr as we don't have one
                            nullptr, // vertical tracking wheel 2, set to nullptr as we don't have one
                            &horizontal, // horizontal tracking wheel 1
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &imu // inertial sensor
);

// create the chassis
lemlib::Differential chassis(drivetrain, linearController, angularController, sensors);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    pros::lcd::initialize(); // initialize brain screen
    chassis.initialize(); // calibrate sensors

    // thread to for brain screen and position logging
    pros::Task screenTask([&]() {
        lemlib::Pose pose(0, 0, 0);
        while (true) {
            pose = chassis.getPose();
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            // delay to save resources
            pros::delay(10);
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
void autonomous() {
    chassis.follow(example_txt, 1.0, 15000);


}

/**
 * Runs in driver control
 */
void opcontrol() {
    // controller
    // loop to continuously update motors
    while (true) {
        // get joystick positions
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
        // move the chassis with curvature drive
        // chassis.curvature(leftY, rightX);

        leftMotors.spinJoystick(leftY);
        rightMotors.spinJoystick(rightY);

        // if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_A)) { testMotor->spinPerc(50); }
        //  delay to save resources
        pros::delay(10);
    }
}