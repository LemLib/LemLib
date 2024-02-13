#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/logger/stdout.hpp"
#include "pros/misc.h"
#include "lemlib/eventhandler/eventhandler.hpp"
#include "lemlib/eventhandler/prosevents/buttonevents.hpp"
#include "pros/misc.hpp"
#include <memory>
#include <vector>

// controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);

<<<<<<< HEAD
<<<<<<< HEAD
// Inertial Sensor on port 2
pros::Imu imu(2);
=======
namespace lemlib {
class ButtonEvent : public Event {
    protected:
        std::function<bool()> function = nullptr;
    public:
        ButtonEvent(std::function<bool()> function) { this->function = function; };

        bool check() override { return function(); }
};
} // namespace lemlib

bool isButtonPressed() {
    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_X)) {
        return true;
    } else {
        return false;
    }
}

=======
>>>>>>> 94cb93f (Update main)
// motor groups
// left motors on ports 8, 20, and 19. Motors on ports 8 and 20 are reversed. Using blue gearbox
auto leftMotors = lemlib::makeMotorGroup({-8, -20, 19}, pros::v5::MotorGears::blue);
// right motors on ports 2, 11, and 13. Motor on port 13 is reversed. Using blue gearbox
auto rightMotors = lemlib::makeMotorGroup({2, 11, -13}, pros::v5::MotorGears::blue);
>>>>>>> a4d44aa (Event Handler restructuring)

// tracking wheels
// horizontal tracking wheel encoder. Rotation sensor, port 15, reversed (negative signs don't work due to a pros bug)
pros::Rotation horizontalEnc(15, true);
// horizontal tracking wheel. 2.75" diameter, 3.7" offset, back of the robot (negative)
lemlib::TrackingWheel horizontal(&horizontalEnc, lemlib::Omniwheel::NEW_275, -3.7);

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
lemlib::Differential chassis(drivetrain, // drivetrain struct
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
    // If you use bluetooth or a wired connection, you will want to have a rate of 10ms

    // for more information on how the formatting for the loggers
    // works, refer to the fmtlib docs
    std::cout << std::setprecision(5);

    // thread to for brain screen and position logging
<<<<<<< HEAD
<<<<<<< HEAD
    pros::Task screenTask([&]() {
        lemlib::Pose pose(0, 0, 0);
        while (true) {
            // print robot location to the brain screen
=======
    pros::Task screenTask([=]() {
=======
    pros::Task screenTask([&]() {
        lemlib::Pose pose(0, 0, 0);
>>>>>>> 7d98df9 (fix odom axis swap)
        while (true) {
            pose = chassis.getPose();
            // print to the brain screen
>>>>>>> c6869de (improve main.cpp documentation)
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            // log position telemetry
            // lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());
            std::cout << "x: " << pose.x << " y: " << pose.y << " theta: " << pose.theta << std::endl;
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
<<<<<<< HEAD
<<<<<<< HEAD
ASSET(example_txt); // '.' replaced with "_" to make c++ happy
=======
ASSET(path_txt); // '.' replaced with "_" to make c++ happy
>>>>>>> c6869de (improve main.cpp documentation)
=======
ASSET(example_txt); // '.' replaced with "_" to make c++ happy
>>>>>>> 170c6ec (Abstract chassis class)

/**
 * Runs during auto
 *
 * This is an example autonomous routine which demonstrates a lot of the features LemLib has to offer
 */
void autonomous() {
<<<<<<< HEAD
    // example movement: Move to x: 20 and y: 15, and face heading 90. Timeout set to 4000 ms
    chassis.moveToPose(20, 15, 90, 4000);
    // example movement: Move to x: 0 and y: 0 and face heading 270, going backwards. Timeout set to 4000ms
    chassis.moveToPose(0, 0, 270, 4000, {.forwards = false});
    // cancel the movement after it has travelled 10 inches
    chassis.waitUntil(10);
    chassis.cancelMotion();
    // example movement: Turn to face the point x:45, y:-45. Timeout set to 1000
    // dont turn faster than 60 (out of a maximum of 127)
    chassis.turnToPoint(45, -45, 1000, {.maxSpeed = 60});
    // example movement: Turn to face a direction of 90º. Timeout set to 1000
    // will always be faster than 100 (out of a maximum of 127)
    chassis.turnToHeading(90, 1000, {.minSpeed = 100});
    // example movement: Follow the path in path.txt. Lookahead at 15, Timeout set to 4000
    // following the path with the back of the robot (forwards = false)
    // see line 116 to see how to define a path
    chassis.follow(example_txt, 15, 4000, false);
=======
    // example movement: Move to x: 20 and y:15, and face heading 90. Timeout set to 4000 ms
    chassis.moveTo(20, 15, 90, 4000);
    // example movement: Turn to face the point x:45, y:-45. Timeout set to 1000
    // dont turn faster than 60 (out of a maximum of 127)
    chassis.turnToPose(45, -45, 1000, true, 60);
    // example movement: Follow the path in path.txt. Lookahead at 15, Timeout set to 4000
    // following the path with the back of the robot (forwards = false)
    // see line 110 to see how to define a path
<<<<<<< HEAD
    chassis.follow(path_txt, 15, 4000, false);
>>>>>>> c6869de (improve main.cpp documentation)
=======
    chassis.follow(example_txt, 15, 4000, false);
>>>>>>> 170c6ec (Abstract chassis class)
    // wait until the chassis has travelled 10 inches. Otherwise the code directly after
    // the movement will run immediately
    // Unless its another movement, in which case it will wait
    chassis.waitUntil(10);
    pros::lcd::print(4, "Travelled 10 inches during pure pursuit!");
    // wait until the movement is done
    chassis.waitUntilDone();
<<<<<<< HEAD
<<<<<<< HEAD
    pros::lcd::print(4, "pure pursuit finished!");
=======
    pros::lcd::print(5, "pure pursuit finished!");
>>>>>>> c6869de (improve main.cpp documentation)
=======
    pros::lcd::print(4, "pure pursuit finished!");
>>>>>>> f312d9b (Rename arcOdom.cpp to differentialArc.cpp)
}

/**
 * Runs in driver control
 */
void opcontrol() {
    std::shared_ptr<pros::Controller> controlla = std::make_shared<pros::Controller>(pros::E_CONTROLLER_MASTER);

    lemlib::PROSButtonEvent XEvent(controlla, pros::E_CONTROLLER_DIGITAL_X, pros::E_CONTROLLER_DIGITAL_X);
    lemlib::PROSButtonEvent BEvent(controlla, pros::E_CONTROLLER_DIGITAL_B, pros::E_CONTROLLER_DIGITAL_B);
    lemlib::PROSButtonEvent YEvent(controlla, pros::E_CONTROLLER_DIGITAL_Y, pros::E_CONTROLLER_DIGITAL_Y);
    lemlib::PROSButtonEvent AEvent(controlla, pros::E_CONTROLLER_DIGITAL_A, pros::E_CONTROLLER_DIGITAL_A);
    lemlib::PROSButtonEvent L1Event(controlla, pros::E_CONTROLLER_DIGITAL_L1, pros::E_CONTROLLER_DIGITAL_L1);
    lemlib::PROSButtonEvent L2Event(controlla, pros::E_CONTROLLER_DIGITAL_L2, pros::E_CONTROLLER_DIGITAL_L2);
    lemlib::PROSButtonEvent R1Event(controlla, pros::E_CONTROLLER_DIGITAL_R1, pros::E_CONTROLLER_DIGITAL_R1);
    lemlib::PROSButtonEvent R2Event(controlla, pros::E_CONTROLLER_DIGITAL_R2, pros::E_CONTROLLER_DIGITAL_R2);

    std::vector<std::shared_ptr<lemlib::Event>> buttonsEvents(
        {std::make_shared<lemlib::PROSButtonEvent>(L1Event), std::make_shared<lemlib::PROSButtonEvent>(L2Event),
         std::make_shared<lemlib::PROSButtonEvent>(BEvent), std::make_shared<lemlib::PROSButtonEvent>(YEvent),
         std::make_shared<lemlib::PROSButtonEvent>(L1Event), std::make_shared<lemlib::PROSButtonEvent>(L2Event),
         std::make_shared<lemlib::PROSButtonEvent>(R1Event), std::make_shared<lemlib::PROSButtonEvent>(R2Event)});

    lemlib::EventHandler eventHandler(buttonsEvents);

    // controller
    // loop to continuously update motors
    while (true) {
        // get joystick positions
<<<<<<< HEAD
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

        eventHandler.getCurrentEvents(pros::E_CONTROLLER_DIGITAL_A);

        // move the chassis with curvature drive
        chassis.curvature(leftY, rightX);
=======
>>>>>>> c6869de (improve main.cpp documentation)
        // delay to save resources
        pros::delay(10);
    }
}