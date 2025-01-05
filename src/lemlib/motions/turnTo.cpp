#include "lemlib/motions/turnTo.hpp"
#include "hardware/Motor/Motor.hpp"
#include "lemlib/MotionCancelHelper.hpp"
#include "LemLog/logger/Helper.hpp"
#include "lemlib/Timer.hpp"
#include "lemlib/util.hpp"
#include "units/Vector2D.hpp"
#include "units/units.hpp"
#include <optional>

logger::Helper logHelper("lemlib/motions/turnToAny");

/**
 * @brief Get the Locked Motors object
 *
 * @param lockedSide Which side is locked
 * @param settings Contains the motor groups
 * @return lemlib::MotorGroup& Locked motor group
 */
static inline lemlib::MotorGroup& getLockedMotors(lemlib::DriveSide lockedSide, lemlib::TurnToAnySettings& settings) {
    return (lockedSide == lemlib::DriveSide::LEFT) ? settings.leftMotors : settings.rightMotors;
};

void lemlib::turnToAny(std::function<Angle()> targetSource, Time timeout, TurnToAnyParams& params,
                       TurnToAnySettings& settings) {
    logHelper.info("Turning to any!");

    // sanitize inputs
    params.minSpeed = abs(params.minSpeed);
    // reset PIDs
    settings.exitConditions.reset();
    settings.angularPID.reset();
    // initialize persistent variables
    std::optional<Angle> previousDirectDeltaTheta = std::nullopt;
    std::optional<Angle> previousDeltaTheta = std::nullopt;
    lemlib::Timer timer(timeout);
    Angle deltaTheta = Angle(INFINITY);
    bool settling = false;
    double previousMotorPower = 0.0;

    // Set brake mode, but remember previous brake mode to reset to
    std::optional<BrakeMode> oldBrakeMode = [&] -> std::optional<BrakeMode> {
        if (auto lockedSide = params.lockedSide)
            return std::make_optional(getLockedMotors(*lockedSide, settings).getBrakeMode());
        else return std::nullopt;
    }();

    // Determine appropriate method to motors based on lockedSide.
    auto moveMotors = [&] -> std::function<void(double)> {
        if (auto lockedSide = params.lockedSide) {
            if (lockedSide == lemlib::DriveSide::LEFT) {
                return [&](double motorPower) {
                    settings.leftMotors.brake();
                    settings.rightMotors.move(-motorPower);
                };
            } else { // lockedSide == lemlib::DriveSide::RIGHT
                return [&](double motorPower) {
                    settings.leftMotors.move(motorPower);
                    settings.rightMotors.brake();
                };
            }
        } else { // lockedSide == std::nullopt
            return [&](double motorPower) {
                settings.leftMotors.move(motorPower);
                settings.rightMotors.move(-motorPower);
            };
        }
    }();

    lemlib::MotionCancelHelper helper(10_msec); // cancel helper
    // loop until the motion has been cancelled, the timer is done, or an exit condition has been met
    while (helper.wait() && !timer.isDone() && !settings.exitConditions.update(deltaTheta)) {
        // get the robot's current position
        const units::Pose pose = settings.poseGetter();

        // calculate deltaTheta
        deltaTheta = [&] {
            const Angle targetTheta = targetSource();

            // Takes the most direct method to get to the target heading, ignoring the direction param.
            const Angle directDeltaTheta = angleError(targetTheta, pose.orientation, AngularDirection::AUTO);

            if (previousDirectDeltaTheta == std::nullopt) previousDirectDeltaTheta = deltaTheta;

            /** Whether directDeltaTheta changed signs */
            const bool directSignChange = units::sgn(directDeltaTheta) != units::sgn(*previousDirectDeltaTheta);
            /** Whether directDeltaTheta is near 0 */
            const bool nearZero = directDeltaTheta < 30_stDeg;
            // If the sign of the direct route changed while near 0, then we must've overshot the target
            settling = settling || (directSignChange && nearZero);

            previousDirectDeltaTheta = directDeltaTheta;
            if (previousDeltaTheta == std::nullopt) previousDeltaTheta = deltaTheta;

            // If we are settling, take the most direct route.
            // This prevents overshooting from resulting in the robot speeding up again past the target heading.
            if (settling) return directDeltaTheta;
            else return angleError(targetTheta, pose.orientation, params.direction);
        }();

        // If motion chaining is enabled
        if (params.minSpeed != 0) {
            // If within exit range, immediately exit this motion
            if (units::abs(deltaTheta) < params.earlyExitRange) break;
            // If passed target, immediately exit this motion
            if (units::sgn(deltaTheta) != units::sgn(previousDeltaTheta.value())) break;
        }
        // calculate speed
        const double motorPower = [&] {
            const double raw = settings.angularPID.update(deltaTheta.convert(deg));
            return lemlib::respectSpeeds(motorPower, previousMotorPower, params.maxSpeed, params.minSpeed,
                                         units::abs(deltaTheta) > 20_stDeg ? settings.angularPID.getGains().slew : 0);
        }();

        moveMotors(motorPower);

        logHelper.debug("Turning with {} power, error: {} deg", motorPower, to_stDeg(deltaTheta));
    }

    settings.leftMotors.move(0);
    settings.rightMotors.move(0);

    if (auto lockedSide = params.lockedSide) getLockedMotors(*lockedSide, settings).setBrakeMode(*oldBrakeMode);

    logHelper.info("Finished turning to any, current heading {} cDeg", to_cDeg(settings.poseGetter().orientation));
}

// TODO: Add logs
void lemlib::turnTo(Angle heading, Time timeout, TurnToAnyParams params, TurnToAnySettings settings) {
    turnToAny([&] { return heading; }, timeout, params, settings);
}

// TODO: Add logs
void lemlib::turnTo(units::Vector2D<Length> target, Time timeout, TurnToPointParams params,
                    TurnToAnySettings settings) {
    const Angle offset = params.forwards ? 0_stDeg : 180_stDeg;
    turnToAny([&] { return settings.poseGetter().angleTo(target) + offset; }, timeout, params, settings);
}