#include "lemlib/MotionCancelHelper.hpp"
#include "pros/rtos.hpp"
#include "pros/misc.h"

namespace lemlib {
MotionCancelHelper::MotionCancelHelper()
    : originalCompStatus(pros::c::competition_get_status()),
      prevTime(pros::millis()) {}

bool MotionCancelHelper::wait(Time timeout) {
    // don't wait if this is the first iteration
    if (firstIteration) {
        timeout = 0_msec;
        firstIteration = false;
    }
    // calculate how long the last iteration took
    Time lastDuration = from_msec(pros::c::millis()) - prevTime;
    // continue if the task has not been notified, and the competition state is the same as when the motion has started
    bool shouldContinue = pros::Task::notify_take(true, lastDuration > timeout ? 0 : to_msec(timeout - lastDuration)) &&
                          pros::c::competition_get_status() == originalCompStatus;
    // record the time this iteration started
    prevTime = from_msec(pros::millis());
    return shouldContinue;
}
} // namespace lemlib