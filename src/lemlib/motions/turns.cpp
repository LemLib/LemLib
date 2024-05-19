#include "lemlib/motions/turns.hpp"
#include "lemlib/util.hpp"
#include "lemlib/logger/logger.hpp"

namespace lemlib {
/**
 * Turn constructor
 *
 * Some members of the class need to be explicitly initialized
 * But, some members need to be configured further in the body
 *
 * Here we just store the arguments in member variables, and store the
 * initial competition state.
 */
DifferentialTurn::DifferentialTurn(PID& angularPID, float target, int timeout, TurnToParams& params)
    : angularPID(angularPID),
      targetHeading(target),
      params(params),
      timer(timeout) {
    // get the current competition state. If this changes, the movement will stop
    this->compState = pros::competition::get_status();
    for (auto e : params.exits) { e.reset(); }
    angularPID.reset();
}

/**
 * Turn constructor
 *
 * Some members of the class need to be explicitly initialized
 * But, some members need to be configured further in the body
 *
 * Here we just store the arguments in member variables, and store the
 * initial competition state.
 */
DifferentialTurn::DifferentialTurn(PID& angularPID, Pose target, int timeout, TurnToParams& params)
    : angularPID(angularPID),
      targetPose(target),
      params(params),
      timer(timeout) {
    // get the current competition state. If this changes, the movement will stop
    this->compState = pros::competition::get_status();
    for (auto e : params.exits) { e.reset(); }
    angularPID.reset();
}

/**
 * The turning algorithm uses field-relative position of the robot to face a target heading
 * or face a target point.
 *
 * This algorithm is simple. When the robot needs to face a target heading, it simply aligns
 * the robot's heading with the target heading. When the robot is turning to face a point,
 * the algorithm will align the robot's heading with the target point. This is better for
 * repeatability, but is not always necessary.
 *
 * This algorithm only uses 1 PID to turn the chassis.
 */
DifferentialChassisSpeeds DifferentialTurn::calculate(Pose pose) {
    switch (state) {
        case 1: return {0, 0};
        case -1: startTheta = (params.forwards) ? fmod(pose.theta, 360) : fmod(pose.theta - 180, 360); state = 0;
    }
    pose.theta = (params.forwards) ? fmod(pose.theta, 360) : fmod(pose.theta - 180, 360);

    // update completion vars
    distTraveled = fabs(angleError(pose.theta, startTheta, false));

    // get the target theta
    float targetTheta = targetPose.has_value() ? pose.angle(targetPose.value()) : targetHeading.value_or(0);

    // check if settling
    const float rawDeltaTheta = angleError(targetTheta, pose.theta, false);
    if (prevRawDeltaTheta == std::nullopt) prevRawDeltaTheta = rawDeltaTheta;
    if (sgn(rawDeltaTheta) != sgn(prevRawDeltaTheta)) settling = true;
    prevRawDeltaTheta = rawDeltaTheta;

    // calculate deltaTheta
    float deltaTheta = 0;
    if (settling) deltaTheta = angleError(targetTheta, pose.theta, false);
    else deltaTheta = angleError(targetTheta, pose.theta, false, params.direction);
    if (prevDeltaTheta == std::nullopt) prevDeltaTheta = deltaTheta;

    // motion chaining
    if (params.minSpeed != 0 && fabs(deltaTheta) < params.earlyExitRange) {
        state = 1;
        return {0, 0};
    };
    if (params.minSpeed != 0 && sgn(deltaTheta) != sgn(prevDeltaTheta)) {
        state = 1;
        return {0, 0};
    };

    // calculate the speed
    float motorPower = angularPID.update(deltaTheta);
    for (auto e : params.exits) { e.update(deltaTheta); }

    // cap the speed
    if (motorPower > params.maxSpeed) motorPower = params.maxSpeed;
    else if (motorPower < -params.maxSpeed) motorPower = -params.maxSpeed;
    if (fabs(deltaTheta) > 20) motorPower = slew(motorPower, prevMotorPower, params.slew);
    if (motorPower < 0 && motorPower > -params.minSpeed) motorPower = -params.minSpeed;
    else if (motorPower > 0 && motorPower < params.minSpeed) motorPower = params.minSpeed;
    prevMotorPower = motorPower;

    infoSink()->debug("Swing Motor Power: {} ", motorPower);

    return {motorPower, -motorPower};
}
}; // namespace lemlib