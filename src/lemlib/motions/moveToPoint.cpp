#include "lemlib/motions/moveToPoint.hpp"
#include "LemLog/logger/Helper.hpp"
#include "lemlib/MotionCancelHelper.hpp"
#include "lemlib/Timer.hpp"
#include "lemlib/util.hpp"

using namespace units;

namespace lemlib {

static logger::Helper logHelper("lemlib/motions/moveToPoint");

void moveToPoint(V2Position target,
                 Time timeout,
                 MoveToPointParams params,
                 MoveToPointSettings settings) {
    logHelper.info("moving to point {}", target);

    // initialize persistent variables
    const Angle initialAngle = settings.poseGetter().angleTo(target);
    Timer timer(timeout);
    bool close = false;
    Number prevLateralOut = 0;
    Number prevAngularOut = 0;
    V2Position lastPose = settings.poseGetter();
    const V2Position normal = target - lastPose;
    const Area k =
      normal * (target + params.earlyExitRange * lastPose.normalize());

    lemlib::MotionCancelHelper helper(10_msec); // cancel helper
    // loop until the motion has been cancelled, or the timer is done
    while (helper.wait() && !timer.isDone()) {
        // get pose
        const Pose pose = settings.poseGetter();

        // check if the robot is close enough to start settling
        if (!close && pose.distanceTo(target) < 7.5_in) {
            close = true;
            params.maxLateralSpeed = max(abs(prevLateralOut), 4.7);
            params.maxAngularSpeed = max(abs(prevLateralOut), 4.7);
        }

        // calculate error
        const Length lateralError =
          pose.distanceTo(target) *
          cos(angleError(pose.orientation, pose.angleTo(target)));
        const Angle angularError = [&] {
            const Angle adjustedTheta =
              params.reversed ? pose.orientation + 180_stDeg : pose.orientation;
            return angleError(adjustedTheta, pose.angleTo(target));
        }();

        // check exit conditions
        if (settings.exitConditions.update(lateralError) && close) break;
        if (params.minLateralSpeed != 0 &&
            ((normal * pose - k) * (normal * lastPose - k)).internal() <= 0) {
            break;
        }
        lastPose = pose;

        // get lateral and angular outputs
        const Number lateralOut = [&] -> Number {
            // get raw output from PID
            auto out = settings.lateralPID.update(to_in(lateralError));
            // apply restrictions on maximum speed
            out = clamp(out, -params.maxLateralSpeed, params.maxLateralSpeed);
            // slew except when settling
            out = close ? out :
                          slew(out,
                               prevLateralOut,
                               params.lateralSlew,
                               helper.getDelta());
            // apply restrictions on minimum speed
            if (!close && params.reversed) {
                out =
                  clamp(out, -params.maxLateralSpeed, -params.minLateralSpeed);
            } else if (!close && !params.reversed) {
                out =
                  clamp(out, params.minLateralSpeed, params.maxLateralSpeed);
            }
            // update previous value
            prevLateralOut = out;
            return out;
        }();
        const Number angularOut = [&] -> Number {
            // if settling, disable turning
            if (close) return 0;
            // get raw output from PID
            auto out = settings.angularPID.update(to_stDeg(angularError));
            // apply restrictions on maximum speed
            out = clamp(out, -params.maxAngularSpeed, params.maxAngularSpeed);
            // slew except when settling
            out =
              slew(out, prevAngularOut, params.angularSlew, helper.getDelta());
            // update previous value
            prevAngularOut = out;
            return out;
        }();

        // print debug info
        logHelper.debug("Moving with {:.4f} lateral power, {:.4f} angular "
                        "power, " "{:.4f} " "lateral error, {:.4f} angular " "e"
                                                                             "r"
                                                                             "r"
                                                                             "o"
                                                                             "r"
                                                                             ","
                                                                             " "
                                                                             "{"
                                                                             ":"
                                                                             "."
                                                                             "4"
                                                                             "f"
                                                                             "}"
                                                                             " "
                                                                             "d"
                                                                             "t",
                        lateralOut,
                        angularOut,
                        lateralError,
                        angularError,
                        helper.getDelta());

        // calculate drivetrain outputs
        const auto out = desaturate(lateralOut, angularOut);
        // move the drivetrain
        settings.leftMotors.move(out.left);
        settings.rightMotors.move(out.right);
    }
}

}; // namespace lemlib
