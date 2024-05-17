#include <cmath>
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/logger/logger.hpp"
#include "lemlib/timer.hpp"
#include "lemlib/util.hpp"
#include "pros/misc.hpp"

void lemlib::Chassis::swingToPoint(float x, float y, DriveSide insideSide, int timeout, SwingToPointParams params,
                                   bool async) {
    params.minSpeed = fabs(params.minSpeed);
    params.swingRadius = fabs(params.swingRadius);
    this->requestMotionStart();
    // were all motions cancelled?
    if (!this->motionRunning) return;
    // if the function is async, run it in a new task
    if (async) {
        pros::Task task([&]() { swingToPoint(x, y, insideSide, timeout, params, false); });
        this->endMotion();
        pros::delay(10); // delay to give the task time to start
        return;
    }
    float targetTheta;
    float deltaX, deltaY, deltaTheta;
    float outsideMotorPower, insideMotorPower;
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
    pros::motor_brake_mode_e brakeMode = (insideSide == DriveSide::LEFT)
                                             ? this->drivetrain.leftMotors->get_brake_modes().at(0)
                                             : this->drivetrain.rightMotors->get_brake_modes().at(0);
    
    // only lock if swing radius is disabled
    if (params.swingRadius == 0) {
        // set brake mode of the locked side to hold
        if (insideSide == DriveSide::LEFT) this->drivetrain.leftMotors->set_brake_modes(pros::E_MOTOR_BRAKE_HOLD);
        else this->drivetrain.rightMotors->set_brake_modes(pros::E_MOTOR_BRAKE_HOLD);
    }

    // main loop
    while (!timer.isDone() && !angularLargeExit.getExit() && !angularSmallExit.getExit() && this->motionRunning) {
        // update variables
        Pose pose = getPose();
        pose.theta = (params.forwards) ? fmod(pose.theta, 360) : fmod(pose.theta - 180, 360);

        // update completion vars
        distTraveled = fabs(angleError(pose.theta, startTheta, false));

        deltaX = x - pose.x;
        deltaY = y - pose.y;
        targetTheta = fmod(radToDeg(M_PI_2 - atan2(deltaY, deltaX)), 360);

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


        // calculate the speed of the outside motors
        outsideMotorPower = angularPID.update(deltaTheta);
        angularLargeExit.update(deltaTheta);
        angularSmallExit.update(deltaTheta);

        // calculate the ratio
        insideMotorPower = 0;
        if (params.swingRadius > 0) {
            double ratio = params.swingRadius / (params.swingRadius + drivetrain.trackWidth);
            insideMotorPower = outsideMotorPower * ratio;
        }

        // cap the speed
        if (outsideMotorPower > params.maxSpeed) outsideMotorPower = params.maxSpeed;
        else if (outsideMotorPower < -params.maxSpeed) outsideMotorPower = -params.maxSpeed;
        if (fabs(deltaTheta) > 20) outsideMotorPower = slew(outsideMotorPower, prevMotorPower, angularSettings.slew);
        if (outsideMotorPower < 0 && outsideMotorPower > -params.minSpeed) outsideMotorPower = -params.minSpeed;
        else if (outsideMotorPower > 0 && outsideMotorPower < params.minSpeed) outsideMotorPower = params.minSpeed;
        prevMotorPower = outsideMotorPower;

        infoSink()->debug("Swing: Outside Motor Power: {}, Inside Motor Power: {} ", outsideMotorPower, insideMotorPower);

        // move the drivetrain
        if (insideSide == DriveSide::LEFT) {
            drivetrain.rightMotors->move(-outsideMotorPower);
            if (params.swingRadius == 0) drivetrain.leftMotors->brake();
            else drivetrain.leftMotors->move(insideMotorPower);
        } else {
            drivetrain.leftMotors->move(outsideMotorPower);
            if (params.swingRadius == 0) drivetrain.rightMotors->brake();
            else drivetrain.rightMotors->move(-insideMotorPower);
        }

        pros::delay(10);
    }

    // set the brake mode of the locked side of the drivetrain to its
    // original value
    if (insideSide == DriveSide::LEFT) this->drivetrain.leftMotors->set_brake_modes(brakeMode);
    else this->drivetrain.rightMotors->set_brake_modes(brakeMode);
    // stop the drivetrain
    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
    // set distTraveled to -1 to indicate that the function has finished
    distTraveled = -1;
    this->endMotion();
}