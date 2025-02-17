#include "lemlib/motions/turnTo.hpp"
#include "lemlib/MotionCancelHelper.hpp"
#include "LemLog/logger/Helper.hpp"
#include "lemlib/Timer.hpp"
#include "lemlib/util.hpp"
#include <optional>
#include <variant>

using namespace units;
using namespace units_double_ops;

namespace lemlib {

static logger::Helper logHelper("lemlib/motions/turnTo");

/**
 * @brief Calculate the error
 *
 * This helper function calculates the angular error.
 *
 * @param target the target
 * @param pose the current pose
 * @param direction what direction to force motion, optionally. Nullopt by default
 *
 * @return Angle the angular error
 */
static Angle calculateError(std::variant<Angle, V2Position> target, const Pose& pose,
                            std::optional<AngularDirection> direction = std::nullopt) {
    // if the target is an angle
    if (std::holds_alternative<Angle>(target)) return angleError(std::get<Angle>(target), pose.orientation, direction);
    // otherwise, the target is a pose
    else return pose.angleTo(std::get<V2Position>(target));
}

void turnTo(std::variant<Angle, V2Position> target, Time timeout, TurnToParams params, TurnToSettings settings) {
    // print debug info
    if (std::holds_alternative<Angle>(target)) logHelper.info("Turning to {:.2f}", std::get<Angle>(target));
    else logHelper.info("Turning to face point {:.2f}", std::get<V2Position>(target));

    // figure out which way to limit acceleration
    const SlewDirection slewDirection = [&] {
        if (params.direction == AngularDirection::CCW_COUNTERCLOCKWISE) return SlewDirection::INCREASING;
        if (params.direction == AngularDirection::CW_CLOCKWISE) return SlewDirection::DECREASING;
        const Pose pose = settings.poseGetter();
        const Angle error = calculateError(target, pose);
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

    // save original brake modes
    const BrakeMode leftBrakeMode = settings.leftMotors.getBrakeMode();
    const BrakeMode rightBrakeMode = settings.rightMotors.getBrakeMode();
    // lock one side of the drivetrain if requested
    if (params.lockedSide) {
        if (*params.lockedSide == TurnToParams::LockedSide::LEFT) settings.leftMotors.setBrakeMode(BrakeMode::BRAKE);
        else settings.rightMotors.setBrakeMode(BrakeMode::BRAKE);
    }

    lemlib::MotionCancelHelper helper(10_msec); // cancel helper
    // loop until the motion has been cancelled, the timer is done, or an exit condition has been met
    while (helper.wait() && !timer.isDone() && !settings.exitConditions.update(deltaTheta)) {
        // get the robot's current position
        const Pose pose = settings.poseGetter();

        // calculate deltaTheta
        deltaTheta = [&] {
            const Angle raw = calculateError(target, pose);
            settling = prevRawDeltaTheta != std::nullopt && (sgn(raw) != sgn(prevRawDeltaTheta.value()));
            prevRawDeltaTheta = raw;
            const Angle error = calculateError(target, pose, settling ? std::nullopt : params.direction);
            if (prevDeltaTheta == std::nullopt) prevDeltaTheta = error;
            return error;
        }();

        // motion chaining
        // exit the motion to immediately continue to the next one
        if (params.minSpeed != 0 && abs(deltaTheta) < params.earlyExitRange) break;
        if (params.minSpeed != 0 && sgn(deltaTheta) != sgn(prevDeltaTheta.value())) break;

        // record prevDeltaTheta
        prevDeltaTheta = deltaTheta;

        // calculate speed
        const Number motorPower = [&] {
            Number raw = settings.angularPID.update(to_stDeg(deltaTheta));
            if (!settling) raw = slew(raw, prevMotorPower, params.slew, helper.getDelta(), slewDirection);
            return constrainPower(raw, params.maxSpeed, params.minSpeed);
        }();

        // record previous motor power
        prevMotorPower = motorPower;

        // print debug info
        logHelper.debug("Turning with {:.4f} power, error: {:.2f} stDeg, dt: {:.4f}", motorPower, to_stDeg(deltaTheta),
                        helper.getDelta());

        // move the motors
        settings.leftMotors.move(-motorPower);
        settings.rightMotors.move(motorPower);
    }

    // apply original brake modes
    settings.leftMotors.setBrakeMode(leftBrakeMode);
    settings.rightMotors.setBrakeMode(rightBrakeMode);

    // stop the drivetrain
    settings.leftMotors.move(0);
    settings.rightMotors.move(0);
}
} // namespace lemlib