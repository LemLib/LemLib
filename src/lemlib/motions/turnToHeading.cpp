#include "lemlib/motions/turnToHeading.hpp"
#include "lemlib/MotionCancelHelper.hpp"
#include "LemLog/logger/Helper.hpp"
#include "lemlib/Timer.hpp"
#include <optional>

namespace lemlib {

static logger::Helper logHelper("lemlib/motions/turnToHeading");

void turnToHeading(Angle targetHeading, Time timeout, lemlib::TurnToHeadingParams params,
                   TurnToHeadingSettings settings) {
    logHelper.info("Turning to {:.2f} cDeg", to_cDeg(targetHeading));

    // sanitize inputs
    params.minSpeed = fabs(params.minSpeed);
    // reset PIDs
    settings.exitConditions.reset();
    settings.angularPID.reset();
    // initialize persistent variables
    std::optional<Angle> previousRawDeltaTheta = std::nullopt;
    std::optional<Angle> previousDeltaTheta = std::nullopt;
    Timer timer(timeout);
    Angle deltaTheta = Angle(INFINITY);
    bool settling = false;
    Number prevMotorPower = 0.0;

    lemlib::MotionCancelHelper helper(10_msec); // cancel helper
    // loop until the motion has been cancelled, the timer is done, or an exit condition has been met
    while (helper.wait() && !timer.isDone() && !settings.exitConditions.update(deltaTheta)) {
        // get the robot's current position
        const units::Pose pose = settings.poseGetter();

        // calculate deltaTheta
        deltaTheta = [&] {
            const Angle raw = angleError(targetHeading, pose.orientation);
            settling = previousRawDeltaTheta && units::sgn(raw) != units::sgn(previousRawDeltaTheta.value());
            previousRawDeltaTheta = raw;
            if (previousDeltaTheta == std::nullopt) previousDeltaTheta = deltaTheta;
            return angleError(targetHeading, pose.orientation, settling ? AngularDirection::AUTO : params.direction);
        }();

        // motion chaining
        // exit the motion to immediately continue to the next one
        if (params.minSpeed != 0 && units::abs(deltaTheta) < params.earlyExitRange) break;
        if (params.minSpeed != 0 && units::sgn(deltaTheta) != units::sgn(previousDeltaTheta.value())) break;

        // calculate speed
        const Number motorPower = [&] {
            const Number raw = settings.angularPID.update(to_stDeg(deltaTheta));
            const Number slewed = slew(motorPower, prevMotorPower, settings.angularPID.getGains().slew);
            return constrainPower(motorPower, params.maxSpeed, params.minSpeed);
        }();

        logHelper.debug("Turning with {:.4f} power, error: {:.2f} stDeg", motorPower, to_stDeg(deltaTheta));

        // move the motors
        settings.leftMotors.move(motorPower);
        settings.rightMotors.move(-motorPower);
    }

    logHelper.info("Finished turning to {:.2f} cDeg, current heading {:.2f} cDeg", to_cDeg(targetHeading),
                   to_cDeg(settings.poseGetter().orientation));

    // stop the drivetrain
    settings.leftMotors.move(0);
    settings.rightMotors.move(0);
}
} // namespace lemlib