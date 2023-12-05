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
Boomerang::Boomerang(FAPID<Length> linearPID, FAPID<Angle> angularPID, Pose target, bool reversed, float chasePower,
                     float lead, int maxSpeed)
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
    if (reversed) this->target.theta = units::mod(this->target.theta + M_PI * rad, 1_rot);
}

/**
 * Get the distance travelled during the movement
 *
 * This is useful if you want to wait until the robot has travelled a certain distance.
 * For example, you want the robot to engage a mechanism when it has travelled 10 inches.
 */
float Boomerang::getDist() { return this->dist.convert(m); } // todo test

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
    if (this->state == 0 && pose.distance(this->target) < 7.5_in) this->state = 1;
    // set state to 2 if in state 1 and the linear PID has settled
    if (this->state == 1 && this->linearPID.settled()) this->state = 2;
    // exit if movement is in state 2 (done)
    if (this->state == 2) return {128, 128};

    // if going in reverse, flip the heading of the pose
    if (this->reversed) pose.theta += M_PI * rad;

    // update completion vars
    if (this->dist == 0_m) { // if dist is 0, this is the first time update() has been called
        this->dist = 0.0001_in;
        this->prevPose = pose;
    }
    this->dist += pose.distance(this->prevPose);
    this->prevPose = pose;

    // calculate the carrot point
    Pose carrot = this->target - (Pose(units::cos(this->target.theta) * m, units::sin(this->target.theta) * m) *
                                  (float)(this->lead * pose.distance(this->target).convert(m)));
    if (this->state == 1) carrot = this->target; // settling behavior

    // calculate error
    Angle angularError = angleError(pose.angle(carrot), pose.theta); // angular error
    Length linearError = pose.distance(carrot);
    // linear error
    if (this->state == 1) linearError *= units::cos(angularError).raw();
    else linearError *= (units::max(units::cos(angularError), 0_num)).raw();
    if (this->state == 1) angularError = angleError(this->target.theta, pose.theta); // settling behavior
    if (this->reversed) linearError = -linearError;

    // get PID outputs
    float angularPower = -this->angularPID.update(angularError, 0_rad); // todo: test
    float linearPower = this->linearPID.update(linearError, 0_m);

    // calculate radius of turn
    Curvature curvature = units::abs(getCurvature(pose, carrot));
    if (curvature == 0_radpm) curvature = -1_radpm;
    Length radius = 1 / curvature;

    // calculate the maximum speed at which the robot can turn
    // using the formula v = sqrt( u * r * g )
    if (radius != -1_m) {
        float maxTurnSpeed = sqrt(this->chasePower * radius.raw() * 9.8);

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
    return {leftPower, rightPower};
}
}; // namespace lemlib