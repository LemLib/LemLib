#include <cmath>
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/logger/logger.hpp"
#include "lemlib/timer.hpp"
#include "lemlib/util.hpp"
#include "pros/misc.hpp"

void lemlib::Chassis::swingToHeading(float theta, DriveSide lockedSide, int timeout, SwingToHeadingParams params,
                                     bool async) {
    params.minSpeed = fabs(params.minSpeed);
    this->requestMotionStart();
    // were all motions cancelled?
    if (!this->motionRunning) return;
    // if the function is async, run it in a new task
    if (async) {
        pros::Task task([&]() { swingToHeading(theta, lockedSide, timeout, params, false); });
        this->endMotion();
        pros::delay(10); // delay to give the task time to start
        return;
    }
    float targetTheta;
    float deltaTheta;
    float motorPower;
    float prevMotorPower = 0;
    float startTheta = getPose().theta;
    bool settling = false;
    std::optional<float> prevRawDeltaTheta = std::nullopt;
    std::optional<float> prevDeltaTheta = std::nullopt;
    std::uint8_t compState = pros::competition::get_status();
    distTraveled = 0;
    Timer timer(timeout);
    angularLargeExit.reset();
    angularSmallExit.reset();
    angularPID.reset();
    // get original braking mode of that side of the drivetrain so we can set it back to it after this motion ends
    pros::MotorBrake brakeMode = (lockedSide == DriveSide::LEFT)
                                     ? this->drivetrain.leftMotors->get_brake_mode_all().at(0)
                                     : this->drivetrain.rightMotors->get_brake_mode_all().at(0);
    // set brake mode of the locked side to hold
    if (lockedSide == DriveSide::LEFT) this->drivetrain.leftMotors->set_brake_mode_all(pros::E_MOTOR_BRAKE_HOLD);
    else this->drivetrain.rightMotors->set_brake_mode_all(pros::E_MOTOR_BRAKE_HOLD);

    // main loop
    while (!timer.isDone() && !angularLargeExit.getExit() && !angularSmallExit.getExit() && this->motionRunning) {
        // update variables
        Pose pose = getPose();
        pose.theta = fmod(pose.theta, 360);

        // update completion vars
        distTraveled = fabs(angleError(pose.theta, startTheta, false));
        targetTheta = theta;

        // check if settling
        const float rawDeltaTheta = angleError(targetTheta, pose.theta, false);
        if (prevRawDeltaTheta == std::nullopt) prevRawDeltaTheta = rawDeltaTheta;
        if (sgn(rawDeltaTheta) != sgn(prevRawDeltaTheta)) settling = true;
        prevRawDeltaTheta = rawDeltaTheta;

        // calculate deltaTheta
        if (settling) deltaTheta = angleError(targetTheta, pose.theta, false);
        else deltaTheta = angleError(targetTheta, pose.theta, false, params.direction);
        if (prevDeltaTheta == std::nullopt) prevDeltaTheta = deltaTheta;

        // motion chaining
        if (params.minSpeed != 0 && fabs(deltaTheta) < params.earlyExitRange) break;
        if (params.minSpeed != 0 && sgn(deltaTheta) != sgn(prevDeltaTheta)) break;

        // calculate the speed
        motorPower = angularPID.update(deltaTheta);
        angularLargeExit.update(deltaTheta);
        angularSmallExit.update(deltaTheta);

        // cap the speed
        if (motorPower > params.maxSpeed) motorPower = params.maxSpeed;
        else if (motorPower < -params.maxSpeed) motorPower = -params.maxSpeed;
        if (fabs(deltaTheta) > 20) motorPower = slew(motorPower, prevMotorPower, angularSettings.slew);
        if (motorPower < 0 && motorPower > -params.minSpeed) motorPower = -params.minSpeed;
        else if (motorPower > 0 && motorPower < params.minSpeed) motorPower = params.minSpeed;
        prevMotorPower = motorPower;

        infoSink()->debug("Turn Motor Power: {} ", motorPower);

        // move the drivetrain
        if (lockedSide == DriveSide::LEFT) {
            drivetrain.rightMotors->move(-motorPower);
            drivetrain.leftMotors->brake();
        } else {
            drivetrain.leftMotors->move(motorPower);
            drivetrain.rightMotors->brake();
        }

        // delay to save resources
        pros::delay(10);
    }

    // set the brake mode of the locked side of the drivetrain to its
    // original value
    if (lockedSide == DriveSide::LEFT) this->drivetrain.leftMotors->set_brake_mode_all(brakeMode);
    else this->drivetrain.rightMotors->set_brake_mode_all(brakeMode);
    // stop the drivetrain
    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
    // set distTraveled to -1 to indicate that the function has finished
    distTraveled = -1;
    this->endMotion();
}