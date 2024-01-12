#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/logger/stdout.hpp"
//#include "lemlib/devices/gamepad.hpp"
#include "pros/misc.h"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"

// controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);
/*
// motor groups
// left motors on ports 8, 20, and 19. Motors on ports 8 and 20 are reversed. Using blue gearbox
auto leftMotors = lemlib::makeMotorGroup({-8, -20, 19}, pros::v5::MotorGears::blue);
// right motors on ports 2, 11, and 13. Motor on port 13 is reversed. Using blue gearbox
auto rightMotors = lemlib::makeMotorGroup({2, 11, -13}, pros::v5::MotorGears::blue);

// Inertial Sensor on port 11
pros::Imu imu(11);

// horizontal tracking wheel. Port 4, 2.75" diameter, 3.7" offset, back of the robot
lemlib::TrackingWheel horizontal(4, lemlib::Omniwheel::NEW_275, -3.7);

// drivetrain settings
lemlib::Drivetrain drivetrain(leftMotors, // left motor group
                              rightMotors, // right motor group
                              10, // 10 inch track width
                              lemlib::Omniwheel::NEW_325, // using new 3.25" omnis
                              360, // drivetrain rpm is 360
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



*/





pros::MotorGroup leftTestMotors({1, 2, -7});
pros::MotorGroup rightTestMotors({4, 5, -6});

pros::Motor intakeMotor(10);


int driveLeft(int percent) {
    leftTestMotors.move_voltage(percent * 12000 / 127);
    //std::cout << "driving left brrrr" << std::endl;
    return percent;
}

int driveRight(int percent) {
    rightTestMotors.move_voltage(percent * 12000 / 127);
    //std::cout << "driving right brrr" << std::endl;
    return percent;
}

int intakeIn(int throwaway) {
    intakeMotor.move(127);
    //std::cout << "intakinggggg" << std::endl;
    return throwaway;
}

int intakeStop(int throwaway) {
    intakeMotor.move(0);
    //std::cout << "stopping intake" << std::endl;
    return throwaway;

}

int debugRPM(int throwaway) {
    std::cout << "RPM: " << leftTestMotors.get_actual_velocity() << std::endl;
    return throwaway;
}

int debugVoltage(int throwaway) {
    std::cout << "Voltage: " << leftTestMotors.get_voltage() << std::endl;
    return throwaway;
}

//lemlib::Gamepad lemController(&controller, {"DEFAULT", "TEST"});

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    pros::lcd::initialize(); // initialize brain screen
    /*chassis.initialize(); // calibrate sensors

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
    });*/
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
    /*// example movement: Move to x: 20 and y:15, and face heading 90. Timeout set to 4000 ms
    chassis.moveTo(20, 15, 90, 4000);
    // example movement: Turn to face the point x:45, y:-45. Timeout set to 1000
    // dont turn faster than 60 (out of a maximum of 127)
    chassis.turnToPose(45, -45, 1000, true, 60);
    // example movement: Follow the path in path.txt. Lookahead at 15, Timeout set to 4000
    // following the path with the back of the robot (forwards = false)
    // see line 116 to see how to define a path
    chassis.follow(example_txt, 15, 4000, false);
    // wait until the chassis has travelled 10 inches. Otherwise the code directly after
    // the movement will run immediately
    // Unless its another movement, in which case it will wait
    chassis.waitUntil(10);
    pros::lcd::print(4, "Travelled 10 inches during pure pursuit!");
    // wait until the movement is done
    chassis.waitUntilDone();
    pros::lcd::print(4, "pure pursuit finished!");*/
}

/**
 * Runs in driver control
 */
void opcontrol() {

    /*lemController.setFuncToAction({intakeStop, intakeIn}, pros::E_CONTROLLER_DIGITAL_A, "DEFAULT");
    lemController.setFuncToAction(driveLeft, pros::E_CONTROLLER_ANALOG_LEFT_Y, "DEFAULT");
    lemController.setFuncToAction(driveRight, pros::E_CONTROLLER_ANALOG_RIGHT_Y, "DEFAULT");
    lemController.setFuncToAction({debugRPM, debugVoltage}, pros::E_CONTROLLER_DIGITAL_B, "TEST");

    lemController.startMainLoop();*/

    // controller 
    // loop to continuously update motors
    while (true) {
        
        //lemController.changeMode("TEST");
        pros::delay(10);
    }
}