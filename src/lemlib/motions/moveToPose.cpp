#include "lemlib/motions/moveToPose.hpp"
#include "LemLog/logger/Helper.hpp"
#include "lemlib/MotionCancelHelper.hpp"
#include "lemlib/Timer.hpp"
#include "lemlib/util.hpp"

using namespace units;

namespace lemlib {
static logger::Helper logHelper("lemlib/motions/moveToPose");

void moveToPose(Pose target,
                Time timeout,
                MoveToPoseParams params,
                MoveToPoseSettings settings) {
    // initialize persistent variables
    Pose lastPose = settings.poseGetter();
    Timer timer(timeout);
    bool close = false;
    Number prevLateralOut = 0;
    Number prevAngularOut = 0;

    lemlib::MotionCancelHelper helper(10_msec);
    // loop until the motion has been cancelled, or the timer is done
    while (helper.wait() && !timer.isDone()) {
        const Pose pose = settings.poseGetter();

        // check if the robot is close enough to the target to start settling
        if (pose.distanceTo(target) < 7.5_in && close == false) {
            close = true;
            params.maxLateralSpeed = max(abs(prevLateralOut), 0.47);
        }

        // find the carrot point
        const V2Position carrot = [&] -> V2Position {
            if (close) return target;
            return target -
                   V2Position::fromPolar(target.orientation,
                                         params.lead * pose.distanceTo(target));
        }();

        // calculate error
        const Length lateralError = [&] {
            Length out = pose.distanceTo(target);
            // only use cosine scaling while settling
            // but always use the sign of the scalar
            const Number scalar =
              cos(angleError(pose.orientation, pose.angleTo(carrot)));
            if (close) {
                out *= scalar;
            } else {
                out *= sgn(scalar);
            }
            return out;
        }();
        const Angle angularError = [&] {
            const Angle adjustedTheta =
              params.reversed ? pose.orientation + 180_stDeg : pose.orientation;
            if (close) {
                return angleError(adjustedTheta, target.orientation);
            } else {
                return angleError(adjustedTheta, pose.angleTo(carrot));
            }
        }();

        // update exit conditions
        if (settings.lateralExitConditions.update(lateralError) &&
            settings.angularExitConditions.update(angularError) && close) {
            break;
        }
    }
}
} // namespace lemlib
