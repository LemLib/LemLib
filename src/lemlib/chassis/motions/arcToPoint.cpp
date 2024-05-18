#include <cmath>
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/logger/logger.hpp"
#include "lemlib/timer.hpp"
#include "lemlib/util.hpp"
#include "pros/misc.hpp"

void lemlib::Chassis::arcToPoint(float x, float y, float radius, int timeout, ArcToPointParams params,
                                     bool async) {
    params.minSpeed = fabs(params.minSpeed);
    this->requestMotionStart();
    // were all motions cancelled?
    if (!this->motionRunning) return;
    // if the function is async, run it in a new task
    if (async) {
        pros::Task task([&]() { arcToPoint(x, y, radius, timeout, params, false); });
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

        // calculate the speed of the inside motors
        insideMotorPower = -outsideMotorPower;
        if (radius != 0) {
            double ratio = (fabs(radius) - 0.5 * drivetrain.trackWidth) / (fabs(radius) + 0.5 * drivetrain.trackWidth);
            insideMotorPower = outsideMotorPower * ratio;
        }

        // cap the speed
        if (outsideMotorPower > params.maxSpeed) outsideMotorPower = params.maxSpeed;
        else if (outsideMotorPower < -params.maxSpeed) outsideMotorPower = -params.maxSpeed;
        if (fabs(deltaTheta) > 20) outsideMotorPower = slew(outsideMotorPower, prevMotorPower, angularSettings.slew);
        if (outsideMotorPower < 0 && outsideMotorPower > -params.minSpeed) outsideMotorPower = -params.minSpeed;
        else if (outsideMotorPower > 0 && outsideMotorPower < params.minSpeed) outsideMotorPower = params.minSpeed;
        prevMotorPower = outsideMotorPower;

        infoSink()->debug("Arc: Outside Motor Power: {}, Inside Motor Power: {} ", outsideMotorPower, insideMotorPower);

        // move the drivetrain
        if (radius < 0) {
            drivetrain.rightMotors->move(-outsideMotorPower);
            drivetrain.leftMotors->move(insideMotorPower);
        } else {
            drivetrain.rightMotors->move(-insideMotorPower);
            drivetrain.leftMotors->move(outsideMotorPower);
        }

        // delay to save resources
        pros::delay(10);    
    }

    // stop the drivetrain
    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
    // set distTraveled to -1 to indicate that the function has finished
    distTraveled = -1;
    this->endMotion();
}