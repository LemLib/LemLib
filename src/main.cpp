#include "main.h"
#include "lemlog/logger/sinks/terminal.hpp"
#include "hardware/Motor/MotorGroup.hpp"
#include "hardware/encoder/ADIEncoder.hpp"
#include "hardware/IMU/V5InertialSensor.hpp"
#include "lemlib/tracking/TrackingWheelOdom.hpp"

logger::Terminal terminal;

lemlib::MotorGroup rightDrive({8, 10}, 360_rpm);
lemlib::MotorGroup leftDrive({18, 19}, 360_rpm);

lemlib::ADIEncoder vEncoder('E', 'F', false);
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
    while (true) {
        auto p = odom.getPose();
        pros::lcd::print(0, "X: %f", to_in(p.x));
        pros::lcd::print(1, "Y: %f", to_in(p.y));
        pros::lcd::print(2, "Theta: %f", to_cDeg(p.orientation));
        pros::delay(10);
    }
}

void disabled() {}

void autonomous() {}

void opcontrol() {}
