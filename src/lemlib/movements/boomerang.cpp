#include <cmath>
#include "pros/misc.hpp"
#include "lemlib/util.hpp"
#include "lemlib/movements/boomerang.hpp"

/**
 * Boomerang constructor
 *
 * Some members of the class need to be explicitly initialized
 * But, some members need to be configured further in the body
 *
 * Here we just store the arguments in member variables, and configure the PIDs
 */
lemlib::Boomerang::Boomerang(FAPID linearPID, FAPID angularPID, Pose target, int timeout, bool forwards,
                             float chasePower, float lead, int maxSpeed)
    : linearPID(linearPID),
      angularPID(angularPID),
      target(target),
      timeout(timeout),
      forwards(forwards),
      chasePower(chasePower),
      lead(lead),
      maxSpeed(maxSpeed) {
    // get the current competition state. If this changes, the movement will stop
    compState = pros::competition::get_status();
    // flip target theta if moving backwards
    if (!forwards) target.theta = fmod(target.theta + M_PI, 2 * M_PI);
}

/**
 * The boomerang controller is a motion algorithm inspired by adaptive PID seeking
 * and pure pursuit. While it can't be used to follow a path, it can be used to
 * move to a pose, that is a point and heading, by moving in a curved path.
 *
 * The algorithm works by calculating the "carrot point". Its calculated using
 * the distance from the robot to the target, multiplying it by the lead distance,
 * and then having it on the same angle from the target as the target heading.
 *
 * This may be more convenient than using pure pursuit, as it doesn't require
 * a path to be generated. Its also more suitable for smaller movements.
 * The controller does have some limitations, however. Since its velocity is
 * not computed in advance, the algorithm has some trouble with sharp turns.
 * The speed around corners should be limited, and this limit can vary greatly
 * between different drivetrains. This is solved by the chasePower parameter.
 * Its used along with an algorithm borrowed from pure pursuit which calculates
 * the curvature of an arc tangent to the heading of the robot and the carrot point,
 * and based on that it will generate a maximum speed the robot can move at.
 *
 * Desmos Demo: https://www.desmos.com/calculator/sptjw5szex
 */
std::pair<int, int> lemlib::Boomerang::update(Pose pose) {
    // set state to 1 if in state 0 and close to the target
    if (state == 0 && pose.distance(target) < 7.5) state = 1;
    // set state to 2 if in state 1 and the linear PID has settled
    if (state == 1 && linearPID.settled()) state = 2;
    // exit if movement is in state 2 (done)
    if (state == 2) return {128, 128};

    // if going in reverse, flip the heading of the pose
    if (!forwards) pose.theta += M_PI;

    // update completion vars
    if (dist == 0) { // if dist is 0, this is the first time update() has been called
        dist = 0.0001;
        lastPose = pose;
    }
    dist += pose.distance(lastPose);
    lastPose = pose;

    // calculate the carrot point
    Pose carrot = target - (Pose(cos(target.theta), sin(target.theta)) * lead * pose.distance(target));
    if (state == 1) carrot = target; // settling behavior

    // calculate error
    float angularError = angleError(pose.angle(carrot), pose.theta, true); // angular error
    float linearError = pose.distance(carrot) * cos(angularError); // linear error
    if (state == 1) angularError = angleError(target.theta, pose.theta, true); // settling behavior
    if (!forwards) linearError = -linearError;

    // get PID outputs
    float angularPower = -angularPID.update(radToDeg(angularError), 0);
    float linearPower = linearPID.update(linearError, 0);

    // calculate radius of turn
    float curvature = fabs(getCurvature(pose, carrot));
    if (curvature == 0) curvature = -1;
    float radius = 1 / curvature;

    // calculate the maximum speed at which the robot can turn
    // using the formula v = sqrt( u * r * g )
    if (radius != -1) {
        float maxTurnSpeed = sqrt(chasePower * radius * 9.8);
        // the new linear power is the minimum of the linear power and the max turn speed
        if (linearPower > maxTurnSpeed && state == 0) linearPower = maxTurnSpeed;
        else if (linearPower < -maxTurnSpeed && state == 0) linearPower = -maxTurnSpeed;
    }

    // prioritize turning over moving
    float overturn = fabs(angularPower) + fabs(linearPower) - maxSpeed;
    if (overturn > 0) linearPower -= linearPower > 0 ? overturn : -overturn;

    // calculate motor powers
    float leftPower = linearPower + angularPower;
    float rightPower = linearPower - angularPower;

    // return motor output
    return {linearPower + angularPower, linearPower - angularPower};
}

/**
 * Get the distance travelled during the movement
 *
 * This is useful if you want to wait until the robot has travelled a certain distance.
 * For example, you want the robot to engage a mechanism when it has travelled 10 inches.
 */
float lemlib::Boomerang::getDist() { return dist; }
