#include "lemlib/motions/moveToPoint.hpp"
#include "LemLog/logger/Helper.hpp"
#include "lemlib/MotionCancelHelper.hpp"
#include "lemlib/Timer.hpp"

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
    std::optional<bool> prevSide = std::nullopt;

    // TODO: embed timeout functionality into MotionCancelHelper

    lemlib::MotionCancelHelper helper(10_msec); // cancel helper
    // loop until the motion has been cancelled, or the timer is done
    while (helper.wait() && !timer.isDone()) {
        // get pose
        const Pose pose = settings.poseGetter();

        // check if the robot is close enough to start settling
        if (!close && pose.distanceTo(target) < 7.5_in) {
            close = true;
            // TODO: fix units, so that _num is not needed
            params.maxSpeed = max(abs(prevLateralOut), 4.7_num);
        }

        // motion chaining
        const bool side = [&] {
            const V2Position diff = pose - target;
        }();
    }
}

}; // namespace lemlib