#include "lemlib/motions/turnToHeading.hpp"
#include "lemlib/MotionCancelHelper.hpp"
#include "LemLog/logger/Helper.hpp"
#include "lemlib/Timer.hpp"
#include <optional>

using namespace units;
using namespace units_double_ops;

namespace lemlib {

static logger::Helper logHelper("lemlib/motions/turnToHeading");

void turnToHeading(Angle targetHeading, Time timeout, lemlib::TurnToHeadingParams params,
                   TurnToHeadingSettings settings) {
    logHelper.info("Turning to {:.2f} cDeg", to_cDeg(targetHeading));

    // sanitize inputs
    params.minSpeed = abs(params.minSpeed);
    // reset PIDs
    settings.exitConditions.reset();
    settings.angularPID.reset();
    // figure out which way to limit acceleration
    const SlewDirection slewDirection = [&] {
        if (params.direction == AngularDirection::CCW_COUNTERCLOCKWISE) return SlewDirection::INCREASING;
        if (params.direction == AngularDirection::CW_CLOCKWISE) return SlewDirection::DECREASING;
        const Angle orientation = settings.poseGetter().orientation;
        const Angle error = angleError(targetHeading, orientation, AngularDirection::AUTO);
        if (error > 0_stDeg) return SlewDirection::INCREASING;
        else return SlewDirection::DECREASING;
    }();
    // initialize persistent variables
    std::optional<Angle> prevRawDeltaTheta = std::nullopt;
    std::optional<Angle> prevDeltaTheta = std::nullopt;
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
            if (prevRawDeltaTheta != std::nullopt && (units::sgn(raw) != units::sgn(prevRawDeltaTheta.value())))
                settling = true;
            prevRawDeltaTheta = raw;
            const Angle error =
                angleError(targetHeading, pose.orientation, settling ? AngularDirection::AUTO : params.direction);
            if (prevDeltaTheta == std::nullopt) prevDeltaTheta = error;
            return error;
        }();

        // motion chaining
        // exit the motion to immediately continue to the next one
        if (params.minSpeed != 0 && units::abs(deltaTheta) < params.earlyExitRange) break;
        if (params.minSpeed != 0 && units::sgn(deltaTheta) != units::sgn(prevDeltaTheta.value())) break;

        // record prevDeltaTheta
        prevDeltaTheta = deltaTheta;

        // calculate speed
        const Number motorPower = [&] {
            Number raw = settings.angularPID.update(to_stDeg(deltaTheta));
            if (!settling) { raw = slew(raw, prevMotorPower, params.slew, helper.getDelta(), slewDirection); }
            return constrainPower(raw, params.maxSpeed, params.minSpeed);
        }();

        // record previous motor power
        prevMotorPower = motorPower;

        logHelper.debug("Turning with {:.4f} power, error: {:.2f} stDeg, dt: {:.4f}", motorPower, to_stDeg(deltaTheta),
                        helper.getDelta());

        // move the motors
        settings.leftMotors.move(-motorPower);
        settings.rightMotors.move(motorPower);
    }

    logHelper.info("Finished turning to {:.2f} cDeg, current heading {:.2f} cDeg", to_cDeg(targetHeading),
                   to_cDeg(settings.poseGetter().orientation));

    // stop the drivetrain
    settings.leftMotors.move(0);
    settings.rightMotors.move(0);

    while (true) { pros::delay(10); }
}
} // namespace lemlib