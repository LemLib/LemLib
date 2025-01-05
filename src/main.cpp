#include "main.h"
#include "hardware/Motor/Motor.hpp"
#include "lemlib/MotionHandler.hpp"
#include "lemlog/logger/sinks/terminal.hpp"
#include "hardware/Motor/MotorGroup.hpp"
#include "hardware/Encoder/ADIEncoder.hpp"
#include "hardware/IMU/V5InertialSensor.hpp"
#include "lemlib/tracking/TrackingWheelOdom.hpp"
#include "lemlib/motions/turnTo.hpp"

logger::Terminal terminal;

lemlib::MotorGroup rightDrive({8, 10}, 360_rpm);
lemlib::MotorGroup leftDrive({18, 19}, 360_rpm);

lemlib::ADIEncoder vEncoder('E', 'F', true);
lemlib::ADIEncoder hEncoder('G', 'H', false);

lemlib::V5InertialSensor imu(1);

lemlib::TrackingWheel rightDriveTracker(&rightDrive, 3.25_in, 26.5_cm / 2);
lemlib::TrackingWheel leftDriveTracker(&leftDrive, 3.25_in, -26.5_cm / 2);

lemlib::TrackingWheel verticalTracker(&vEncoder, 2.75_in, 0_in);
lemlib::TrackingWheel horizontalTracker(&hEncoder, 2.75_in, 0_in);

lemlib::TrackingWheelOdometry odom({&imu}, {verticalTracker, rightDriveTracker, leftDriveTracker}, {horizontalTracker});

void initialize() {
    terminal.setLoggingLevel(logger::Level::DEBUG);
    pros::lcd::initialize();
    imu.calibrate();
    pros::delay(3200);
    odom.startTask();
    pros::delay(100);
    odom.setPose({0_in, 0_in, 45_cDeg});
    while (true) {
        auto p = odom.getPose();
        pros::lcd::print(0, "X: %f", to_in(p.x));
        pros::lcd::print(1, "Y: %f", to_in(p.y));
        pros::lcd::print(2, "Theta: %f", to_cDeg(p.orientation));
        pros::delay(10);
    }
}

void disabled() {}

void autonomous() {
    using lemlib::motion_handler::move;
    odom.setPose({0_in, 0_in, 0_cDeg});

    leftDrive.setBrakeMode(lemlib::BrakeMode::COAST);
    rightDrive.setBrakeMode(lemlib::BrakeMode::COAST);

    lemlib::TurnToAnySettings turnToSettings {
        .angularPID = lemlib::PID(0, 0, 0),
        .exitConditions = lemlib::ExitConditionGroup<AngleRange>(std::vector<lemlib::ExitCondition<AngleRange>> {}),
        .poseGetter = [&] { return odom.getPose(); },
        .leftMotors = leftDrive,
        .rightMotors = rightDrive};

    // turnToHeading
    move([&] { lemlib::turnTo(-90_cDeg, 1000_msec, {}, turnToSettings); });

    // turnToPoint (params syntax needs work)
    move([&] { lemlib::turnTo({0_in, 100_in}, 1000_msec, {lemlib::TurnToAnyParams {}, false}, turnToSettings); });

    // swingToHeading
    move([&] {
        lemlib::turnTo(0_cDeg, 1000_msec,
                       {
                           .lockedSide = lemlib::DriveSide::LEFT,
                           .minSpeed = 1,
                       },
                       turnToSettings);
    });

    // swingToPoint (params syntax needs work)
    move([&] {
        lemlib::turnTo(units::Vector2D {1000_kmps2, .1_mph2} * 10_day * .1_sec, 1000_msec,
                       {lemlib::TurnToAnyParams {
                           .lockedSide = lemlib::DriveSide::RIGHT,
                           .direction = lemlib::AngularDirection::CCW_COUNTERCLOCKWISE,
                           .maxSpeed = .5,
                       }},
                       turnToSettings);
    });

    // Once motions are over, test if motors are still in coast mode
}

void opcontrol() {}
