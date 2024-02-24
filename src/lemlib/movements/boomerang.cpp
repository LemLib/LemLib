#include <cmath>
#include "pros/misc.hpp"
#include "lemlib/util.hpp"
#include "lemlib/movements/boomerang.hpp"

namespace lemlib {
/**
 * Boomerang constructor
 *
 * Some members of the class need to be explicitly initialized
 * But, some members need to be configured further in the body
 *
 * Here we just store the arguments in member variables, and store the
 * initial competition state, and manipulate the target heading based on
 * whether the robot is going to be moving forwards or backwards
 */
Boomerang::Boomerang(FAPID linearPID, FAPID angularPID, Pose target, bool reversed, float chasePower, float lead,
                     int maxSpeed)
    : Movement(),
      linearPID(linearPID),
      angularPID(angularPID),
      target(target),
      reversed(reversed),
      chasePower(chasePower),
      lead(lead),
      maxSpeed(maxSpeed) {
    // get the current competition state. If this changes, the movement will stop
    this->compState = pros::competition::get_status();
    // flip target theta if moving backwards
    if (reversed) this->target.theta = fmod(this->target.theta + M_PI, 2 * M_PI);
}

/**
 * Get the distance travelled during the movement
 *
 * This is useful if you want to wait until the robot has travelled a certain distance.
 * For example, you want the robot to engage a mechanism when it has travelled 10 inches.
 */
float Boomerang::getDist() { return this->dist; }

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
std::pair<int, int> Boomerang::update(Pose pose) {
    // set state to 1 if in state 0 and close to the target
    if (this->state == 0 && pose.distance(this->target) < 7.5) this->state = 1;
    // set state to 2 if in state 1 and the linear PID has settled
    if (this->state == 1 && this->linearPID.settled()) this->state = 2;
    // exit if movement is in state 2 (done)
    if (this->state == 2) return {128, 128};

    // if going in reverse, flip the heading of the pose
    if (this->reversed) pose.theta += M_PI;

    // update completion vars
    if (this->dist == 0) { // if dist is 0, this is the first time update() has been called
        this->dist = 0.0001;
        this->prevPose = pose;
    }
    this->dist += pose.distance(this->prevPose);
    this->prevPose = pose;

    // calculate the carrot point
    Pose carrot = this->target -
                  (Pose(cos(this->target.theta), sin(this->target.theta)) * this->lead * pose.distance(this->target));
    if (this->state == 1) carrot = this->target; // settling behavior

    // calculate error
    float angularError = angleError(pose.angle(carrot), pose.theta); // angular error
    float linearError = pose.distance(carrot);
    // linear error
    if (this->state == 1) linearError *= cos(angularError);
    else linearError *= std::fmax(cos(angularError), 0);
    if (this->state == 1) angularError = angleError(this->target.theta, pose.theta); // settling behavior
    if (this->reversed) linearError = -linearError;

    // get PID outputs
    float angularPower = -this->angularPID.update(radToDeg(angularError), 0);
    float linearPower = this->linearPID.update(linearError, 0);

    // calculate radius of turn
    float curvature = fabs(getCurvature(pose, carrot));
    if (curvature == 0) curvature = -1;
    float radius = 1 / curvature;

    // calculate the maximum speed at which the robot can turn
    // using the formula v = sqrt( u * r * g )
    if (radius != -1) {
        float maxTurnSpeed = sqrt(this->chasePower * radius * 9.8);
        // the new linear power is the minimum of the linear power and the max turn speed
        if (linearPower > maxTurnSpeed && this->state == 0) linearPower = maxTurnSpeed;
        else if (linearPower < -maxTurnSpeed && this->state == 0) linearPower = -maxTurnSpeed;
    }

    // prioritize turning over moving
    float overturn = fabs(angularPower) + fabs(linearPower) - this->maxSpeed;
    if (overturn > 0) linearPower -= linearPower > 0 ? overturn : -overturn;

    // calculate motor powers
    float leftPower = linearPower + angularPower;
    float rightPower = linearPower - angularPower;

    // return motor output
    return {linearPower + angularPower, linearPower - angularPower};
}
}; // namespace lemlib