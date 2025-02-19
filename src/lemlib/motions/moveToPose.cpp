#include "lemlib/motions/moveToPose.hpp"
#include "LemLog/logger/Helper.hpp"
#include "lemlib/MotionCancelHelper.hpp"
#include "lemlib/Timer.hpp"
#include "lemlib/util.hpp"

using namespace units;

namespace lemlib {

static logger::Helper logHelper("lemlib/motions/moveToPose");

void moveToPose(Pose target, Time timeout, MoveToPoseParams params, MoveToPoseSettings settings) {
    // initialize persistent variables
    Pose lastPose = settings.poseGetter();
    Timer timer(timeout);
    bool close = false;
    bool prevSameSide = false;
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
            return target - V2Position::fromPolar(target.orientation, params.lead * pose.distanceTo(target));
        }();

        // calculate error
        const Length lateralError = [&] {
            Length out = pose.distanceTo(target);
            // only use cosine scaling while settling
            // but always use the sign of the scalar
            const Number scalar = cos(angleError(pose.orientation, pose.angleTo(carrot)));
            if (close) out *= scalar;
            else out *= sgn(scalar);
            return out;
        }();
        const Angle angularError = [&] {
            const Angle adjustedTheta = params.reversed ? pose.orientation + 180_stDeg : pose.orientation;
            if (close) return angleError(adjustedTheta, target.orientation);
            else return angleError(adjustedTheta, pose.angleTo(carrot));
        }();

        // check exit conditions
        if (settings.lateralExitConditions.update(lateralError) &&
            settings.angularExitConditions.update(angularError) && close) {
            break;
        }
        {
            const bool robotSide = (pose.y - target.y) * -sin(target.orientation) <=
                                   (pose.x - target.x) * cos(target.orientation) + params.earlyExitRange;
            const bool carrotSide = (carrot.y - target.y) * -sin(target.orientation) <=
                                    (carrot.x - target.x) * cos(target.orientation) + params.earlyExitRange;
            const bool sameSide = robotSide == carrotSide;
            // exit if close
            if (!sameSide && prevSameSide && close && params.minLateralSpeed != 0) break;
            prevSameSide = sameSide;
        }

        // get lateral and angular outputs
        const Number angularOut = [&] -> Number {
            // get output from PID
            Number out = settings.angularPID.update(to_stRad(angularError));
            // restrict maximum speed
            out = clamp(out, -params.maxAngularSpeed, params.maxAngularSpeed);
            // update prevAngularOut
            prevAngularOut = out;
            return out;
        }();
        const Number lateralOut = [&] -> Number {
            // get output from PID
            Number out = settings.lateralPID.update(to_m(lateralError));
            // restrict maximum speed
            out = clamp(out, -params.maxLateralSpeed, params.maxLateralSpeed);
            // limit acceleration
            if (!close) out = slew(out, prevLateralOut, params.lateralSlew, helper.getDelta());
            // prevent slipping
            const Length radius = 1 / abs(getSignedTangentArcCurvature(pose, carrot));
            const Number maxSlipSpeed = sqrt(params.driftCompensation * to_m(radius));
            out = clamp(out, -maxSlipSpeed, maxSlipSpeed);
            // prioritize angular movement over lateral movement
            const Number overturn = abs(angularOut) + abs(out) - params.maxLateralSpeed;
            if (overturn > 0) out -= out > 0 ? overturn : -overturn;
            // prevent moving in the wrong direction
            if (params.reversed && !close) out = units::min(out, 0);
            else if (!params.reversed && !close) out = max(out, 0);
            // constrain by minimum speed
            if (params.reversed && -out < abs(params.minLateralSpeed) && out < 0) out = -abs(params.minLateralSpeed);
            else if (!params.reversed && out < abs(params.minLateralSpeed) && out > 0)
                out = abs(params.minLateralSpeed);
            // update prevLateralOut
            prevLateralOut = out;
            return out;
        }();

        // print debug info
        logHelper.debug("Moving with {:.4f} lateral power, {:.4f} angular power, {:.4f} lateral error, {:.4f} angular "
                        "error, {:.4f} dt",
                        lateralOut, angularOut, lateralError, angularError, helper.getDelta());

        // calculate drivetrain outputs
        const auto out = desaturate(lateralOut, angularOut);
        // move the drivetrain
        settings.leftMotors.move(out.left);
        settings.rightMotors.move(out.right);
    }
}
} // namespace lemlib
