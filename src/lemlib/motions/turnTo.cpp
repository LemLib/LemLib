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

void lemlib::turnToAny(std::function<Angle()> targetSource, std::function<void(double)> outputSetter, Time timeout,
                       TurnToAnyParams& params, TurnToAnySettings& settings) {
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

        // move the motors
        outputSetter(motorPower);

        logHelper.debug("Turning with {} power, error: {} deg", motorPower, to_stDeg(deltaTheta));
    }

    settings.leftMotors.move(0);
    settings.rightMotors.move(0);

    logHelper.info("Finished turning to any, current heading {} cDeg", to_cDeg(settings.poseGetter().orientation));
}

static std::function<void(double)> createOutputForTurn(lemlib::TurnToAnySettings& settings) {
    return [&](double motorPower) {
        settings.leftMotors.move(motorPower);
        settings.rightMotors.move(-motorPower);
    };
}

static std::function<void(double)> createOutputForSwing(lemlib::DriveSide lockedSide,
                                                        lemlib::TurnToAnySettings& settings) {
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
}

static std::function<Angle(void)> createTargetSourceForHeading(Angle target) {
    return [&] { return target; };
}

static std::function<Angle(void)> createTargetSourceForPoint(units::Vector2D<Length> target,
                                                             lemlib::TurnToPointParams& params,
                                                             lemlib::TurnToAnySettings& settings) {
    const Angle offset = params.forwards ? 0_stDeg : 180_stDeg;
    return [&] { return settings.poseGetter().angleTo(target) + offset; };
}

void lemlib::turnToHeading(Angle heading, Time timeout, TurnToAnyParams params, TurnToAnySettings settings) {
    turnToAny(createTargetSourceForHeading(heading), createOutputForTurn(settings), timeout, params, settings);
}

void lemlib::turnToPoint(units::Vector2D<Length> target, Time timeout, TurnToPointParams params,
                         TurnToAnySettings settings) {
    turnToAny(createTargetSourceForPoint(target, params, settings), createOutputForTurn(settings), timeout, params,
              settings);
}

void lemlib::swingToHeading(Angle target, DriveSide lockedSide, Time timeout, TurnToAnyParams params,
                            TurnToAnySettings settings) {
    MotorGroup& lockedMotorGroup = (lockedSide == DriveSide::LEFT) ? settings.leftMotors : settings.rightMotors;
    // Remember previous brake mode to reset to
    BrakeMode oldBrakeMode = lockedMotorGroup.getBrakeMode();

    // Lock locked side of drive
    lockedMotorGroup.setBrakeMode(BrakeMode::HOLD);

    turnToAny(createTargetSourceForHeading(target), createOutputForSwing(lockedSide, settings), timeout, params,
              settings);

    // Reset brake mode
    lockedMotorGroup.setBrakeMode(oldBrakeMode);
}

void lemlib::swingToPoint(units::Vector2D<Length> target, DriveSide lockedSide, Time timeout, TurnToPointParams params,
                          TurnToAnySettings settings) {
    MotorGroup& lockedMotorGroup = (lockedSide == DriveSide::LEFT) ? settings.leftMotors : settings.rightMotors;
    // Remember previous brake mode to reset to
    BrakeMode oldBrakeMode = lockedMotorGroup.getBrakeMode();

    // Lock locked side of drive
    lockedMotorGroup.setBrakeMode(BrakeMode::HOLD);

    turnToAny(createTargetSourceForPoint(target, params, settings), createOutputForSwing(lockedSide, settings), timeout,
              params, settings);

    // Reset brake mode
    lockedMotorGroup.setBrakeMode(oldBrakeMode);
}