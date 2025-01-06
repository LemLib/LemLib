#include "main.h"
#include "lemlog/logger/sinks/terminal.hpp"
#include "hardware/Motor/MotorGroup.hpp"
#include "hardware/IMU/V5InertialSensor.hpp"
#include "lemlib/tracking/TrackingWheelOdom.hpp"
#include "lemlib/motions/turnToHeading.hpp"
#include "pros/llemu.hpp"

logger::Terminal terminal;

lemlib::MotorGroup rightDrive({8, 10}, 360_rpm);
lemlib::MotorGroup leftDrive({-18, -19}, 360_rpm);

lemlib::V5InertialSensor imu(1);

lemlib::TrackingWheel verticalTracker('E', 'F', true, 2.75_in, 26.5_cm / 2);
lemlib::TrackingWheel horizontalTracker('G', 'H', false, 2.75_in, -26.5_cm / 2);

lemlib::TrackingWheelOdometry odom({&imu}, {&verticalTracker}, {&horizontalTracker});

lemlib::PID pid(0.05, 0, 0);
lemlib::ExitCondition<AngleRange> exitCondition(1_stDeg, 2_sec);

void initialize() {
    terminal.setLoggingLevel(logger::Level::DEBUG);
    pros::lcd::initialize();

    imu.calibrate();
    pros::delay(3200);
    odom.startTask();
    pros::delay(100);
    pros::Task([&] {
        while (true) {
            auto p = odom.getPose();
            pros::lcd::print(0, "X: %f", to_in(p.x));
            pros::lcd::print(1, "Y: %f", to_in(p.y));
            pros::lcd::print(2, "Theta: %f", to_cDeg(p.orientation));
            pros::delay(10);
        }
    });
    lemlib::turnToHeading(90_cDeg, 100_sec, {},
                          {
                              .angularPID = pid,
                              .exitConditions = std::vector<lemlib::ExitCondition<AngleRange>>({exitCondition}),
                              .poseGetter = [] -> units::Pose { return odom.getPose(); },
                              .leftMotors = leftDrive,
                              .rightMotors = rightDrive,
                          });
}

void disabled() {}

void autonomous() {}

void opcontrol() {}
