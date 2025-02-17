#include "lemlib/motions/moveToPoint.hpp"
#include "LemLog/logger/Helper.hpp"
#include "lemlib/MotionCancelHelper.hpp"
#include "lemlib/Timer.hpp"
#include "lemlib/util.hpp"

using namespace units;
using namespace units_double_ops;

namespace lemlib {

static logger::Helper logHelper("lemlib/motions/moveToPoint");

void moveToPoint(units::V2Position target, Time timeout, MoveToPointParams params, MoveToPointSettings settings) {
    // TODO: print debug info
    // logHelper.info(...)

    // initialize persistent variables
    const Angle initialAngle = settings.poseGetter().angleTo(target);
    Timer timer(timeout);
    bool close = false;
    Number prevLateralOut = 0;
    Number prevAngularOut = 0;
    V2Position lastPose = settings.poseGetter();
    const V2Position normal = target - lastPose;
    const Area k = normal * (target + params.earlyExitRange * lastPose.normalize());

    lemlib::MotionCancelHelper helper(10_msec); // cancel helper
    // loop until the motion has been cancelled, or the timer is done
    while (helper.wait() && !timer.isDone()) {
        // get pose
        const Pose pose = settings.poseGetter();

        // check if the robot is close enough to start settling
        if (!close && pose.distanceTo(target) < 7.5_in) {
            close = true;
            // TODO: fix units, so that _num is not needed
            params.maxLateralSpeed = max(abs(prevLateralOut), 4.7_num);
            params.maxAngularSpeed = max(abs(prevLateralOut), 4.7_num);
        }

        // calculate error
        const Angle angularError = [&] {
            const Angle adjustedTheta = params.reversed ? pose.orientation + 180_stDeg : pose.orientation;
            return angleError(adjustedTheta, pose.angleTo(target));
        }();
        const Length lateralError = pose.distanceTo(target) * cos(angleError(pose.orientation, pose.angleTo(target)));

        // check exit conditions
        if (settings.exitConditions.update(lateralError)) break;
        if (params.minLateralSpeed != 0 && ((normal * pose - k) * (normal * lastPose - k)).internal() <= 0) break;
        lastPose = pose;

        // get lateral and angular outputs
        const Number lateralOut = [&] {
            // get raw output from PID
            auto out = settings.lateralPID.update(to_in(lateralError));
            // apply restrictions on maximum speed
            out = clamp(out, -params.maxLateralSpeed, params.maxLateralSpeed);
            // slew except when settling
            out = close ? out : slew(out, prevLateralOut, params.lateralSlew, helper.getDelta());
            // apply restrictions on minimum speed
            if (!close && params.reversed) out = clamp(out, -params.maxLateralSpeed, -params.minLateralSpeed);
            else if (!close && !params.reversed) out = clamp(out, params.minLateralSpeed, params.maxLateralSpeed);
            // update previous value
            prevLateralOut = out;
            return out;
        }();
        const Number angularOut = [&] {
            // if settling, disable turning
            if (close) return 0_num;
            // get raw output from PID
            auto out = settings.angularPID.update(to_stDeg(angularError));
            // apply restrictions on maximum speed
            out = clamp(out, -params.maxAngularSpeed, params.maxAngularSpeed);
            // slew except when settling
            out = slew(out, prevAngularOut, params.angularSlew, helper.getDelta());
            // update previous value
            prevAngularOut = out;
            return out;
        }();

        // calculate drivetrain outputs
        const auto out = desaturate(lateralOut, angularOut);
        // move the drivetrain
        settings.leftMotors.move(out.left);
        settings.rightMotors.move(out.right);
    }
}

}; // namespace lemlib