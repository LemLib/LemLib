#include "lemlib/MotionCancelHelper.hpp"
#include "pros/rtos.hpp"
#include "pros/misc.h"

namespace lemlib {
MotionCancelHelper::MotionCancelHelper()
    : originalCompStatus(pros::c::competition_get_status()),
      prevTime(pros::millis()) {}

bool MotionCancelHelper::wait(Time timeout) {
    /** Duration of last iteration of the motion's loop */
    Time lastDuration = from_msec(pros::c::millis()) - prevTime;
    bool shouldCancel = !pros::Task::notify_take(true, lastDuration > timeout ? 0 : to_msec(timeout - lastDuration)) ||
                        pros::c::competition_get_status() != originalCompStatus;
    prevTime = from_msec(pros::millis());
    return shouldCancel;
}
} // namespace lemlib