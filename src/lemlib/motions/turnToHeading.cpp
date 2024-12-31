#include "lemlib/motions/turnToHeading.hpp"
#include "pros/apix.h"
#include <optional>

void lemlib::turnToHeading(Angle heading, Time timeout, lemlib::TurnToHeadingParams params,
                           lemlib::TurnToHeadingSettings settings) {
    params.minSpeed = abs(params.minSpeed);

    // TODO: Motion handler stuff
    // TODO: Timer

    settings.angularLargeExit.reset();
    settings.angularSmallExit.reset();
    settings.angularPID.reset();

    std::optional<Angle> previousRawDeltaTheta = std::nullopt;
    std::optional<Angle> previousDeltaTheta = std::nullopt;
    Angle startingTheta = settings.poseGetter().theta();
    Angle angleTraveled = 0_stRot;
    Angle targetTheta = heading;
    Angle deltaTheta = 0_stRot;

    bool settling = false;

    double previousMotorPower = 0.0;
    double motorPower = 0.0;

    while (!settings.angularLargeExit.update(deltaTheta.convert(deg)) &&
           !settings.angularSmallExit.update(deltaTheta.convert(deg))) {
        // get the robot's current position
        units::Pose pose = settings.poseGetter();

        // update angle traveled
        angleTraveled = units::abs(lemlib::angleError(pose.theta(), startingTheta));

        targetTheta = heading;

        const Angle rawDeltaTheta = angleError(targetTheta, pose.theta());
        settling = previousRawDeltaTheta && units::sgn(rawDeltaTheta) != units::sgn(previousRawDeltaTheta.value());
        previousRawDeltaTheta = rawDeltaTheta;

        deltaTheta = angleError(targetTheta, pose.theta(), settling ? AngularDirection::AUTO : params.direction);
        if (previousDeltaTheta == std::nullopt) previousDeltaTheta = deltaTheta;

        // motion chaining
        // exit the motion to immediately continue to the next one
        if (params.minSpeed != 0 && units::abs(deltaTheta) < params.earlyExitRange) break;
        if (params.minSpeed != 0 && units::sgn(deltaTheta) != units::sgn(previousDeltaTheta.value())) break;

        // calculate speed
        motorPower = settings.angularPID.update(deltaTheta.convert(deg));

        if (motorPower > params.maxSpeed) motorPower = params.maxSpeed;
        else if (motorPower < -params.maxSpeed) motorPower = -params.maxSpeed;
        if (units::abs(deltaTheta) > 20_stDeg)
            motorPower = lemlib::slew(motorPower, previousMotorPower, settings.angularPID.getGains().slew);
        if (motorPower < 0 && motorPower > -params.minSpeed) motorPower = -params.minSpeed;
        else if (motorPower > 0 && motorPower < params.minSpeed) motorPower = params.minSpeed;
        previousMotorPower = motorPower;

        // move the motors
        // TODO: add drivetrain

        pros::delay(10);
    }

    // stop the drivetrain
    // TODO: add drivetrain

    // TODO: do stuff with angle traveled

    // TODO: end the motion
}