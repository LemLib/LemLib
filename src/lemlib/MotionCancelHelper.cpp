#include "lemlib/MotionCancelHelper.hpp"
#include "pros/rtos.hpp"
#include "pros/misc.h"

namespace lemlib {
MotionCancelHelper::MotionCancelHelper()
    : originalCompStatus(pros::c::competition_get_status()),
      prevTime(from_msec(pros::millis())) {}

bool MotionCancelHelper::wait(Time timeout) {
    const Time now = from_msec(pros::millis());
    // don't wait if this is the first iteration
    if (firstIteration) {
        timeout = 0_msec;
        firstIteration = false;
    }

    // if the competition state is not the same as when the motion started, then stop the motion
    if (pros::c::competition_get_status() != originalCompStatus) return 0;

    const Time lastDuration = now - prevTime; // calculate how long the last iteration took
    // check how long to wait for a notification
    const Time notificationTimeout = (lastDuration > timeout) ? 0_msec : timeout - lastDuration;
    // check if there was a notification
    const bool shouldContinue = pros::Task::notify_take(true, to_msec(notificationTimeout)) == 0;
    prevTime = from_msec(pros::millis());
    return shouldContinue;
}
} // namespace lemlib