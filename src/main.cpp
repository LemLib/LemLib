#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/logger/stdout.hpp"

// left drive motors on ports 8, 20, 18. Motors on ports 8 and 20 are reversed
auto leftDrive = lemlib::makeMotorGroup({-8, -20, 19}, pros::v5::MotorGears::blue);
// right drive motors on ports 2, 11, 13. Motor on port 13 is reversed
auto rightDrive = lemlib::makeMotorGroup({2, 11, -13}, pros::v5::MotorGears::blue);

// Inertial Sensor on port 12
pros::Imu imu(12);

// vertical tracking wheel. Port 4, not reversed, 2.75" diameter, 3.7" offset, left of the robot center
lemlib::TrackingWheel vertical(4, lemlib::Omniwheel::NEW_275, -3.7);

// drivetrain
lemlib::Drivetrain_t drivetrain {
    leftDrive, // left drivetrain motors
    rightDrive, // right drivetrain motors
    10, // track width of robot (distance from left wheels to distance of right)
    lemlib::Omniwheel::NEW_4, // wheel diameter
    300, // rpm of the drivetrain
    8 // chase power. Higher values result in sharper turns
};

// lateral motion controller
lemlib::ChassisController_t lateralController {
    10, // kP
    30, // kD
    1, // small exit range
    100, // small exit timeout
    3, // large error range
    500, // large error timeout
    20 // acceleration cap
};

// angular motion controller
lemlib::ChassisController_t angularController {
    2, // kP
    10, // kD
    1, // small exit range
    100, // small exit timeout
    3, // large exit range
    500, // large exit timeout
    20 // acceleration cap
};

// sensors for odometry
lemlib::OdomSensors_t sensors {
    &vertical, // vertical tracking wheel
    nullptr, // we don't have a second vertical tracking wheel
    nullptr, // we don't have a horizontal tracking wheel
    nullptr, // we don't have a horizontal tracking wheel
    &imu // inertial sensor (AKA imu)
};

// chassis
lemlib::Chassis chassis(drivetrain, // drivetrain struct
                        lateralController, // forwards/backwards PID struct
                        angularController, // turning PID struct
                        sensors // sensors struct
);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    pros::lcd::initialize(); // initialize brain screen
    chassis.initialize(); // calibrate sensors

    // the default rate is 50. however, if you need to change the rate, you
    // can do the following.
    // lemlib::bufferedStdout().setRate(...);
    // If you use bluetooth or a wired connection, you will want to have a rate of 10ms

    // for more information on how the formatting for the loggers
    // works, refer to the fmtlib docs

    // thread to for brain screen and position logging
    pros::Task screenTask([=]() {
        while (true) {
            // print to the brain screen
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
ASSET(path_txt); // '.' replaced with "_" to make c++ happy

/**
 * Runs during auto
 *
 * This is an example autonomous routine which demonstrates a lot of the features LemLib has to offer
 */
void autonomous() {
    // example movement: Move to x: 20 and y:15, and face heading 90. Timeout set to 4000 ms
    chassis.moveTo(20, 15, 90, 4000);
    // example movement: Turn to face the point x:45, y:-45. Timeout set to 1000
    // dont turn faster than 60 (out of a maximum of 127)
    chassis.turnToPose(45, -45, 1000, true, 60);
    // example movement: Follow the path in path.txt. Lookahead at 15, Timeout set to 4000
    // following the path with the back of the robot (forwards = false)
    // see line 110 to see how to define a path
    chassis.follow(path_txt, 15, 4000, false);
    // wait until the chassis has travelled 10 inches. Otherwise the code directly after
    // the movement will run immediately
    // Unless its another movement, in which case it will wait
    chassis.waitUntil(10);
    pros::lcd::print(4, "Travelled 10 inches during pure pursuit!");
    // wait until the movement is done
    chassis.waitUntilDone();
    pros::lcd::print(5, "pure pursuit finished!");
}

/**
 * Runs in driver control
 */
void opcontrol() {
    // controller
    // loop to continuously update motors
    while (true) {
        // get joystick positions
        // delay to save resources
        pros::delay(10);
    }
}
