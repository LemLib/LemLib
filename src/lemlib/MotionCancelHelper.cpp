#include "lemlib/MotionCancelHelper.hpp"
#include "pros/rtos.hpp"
#include "pros/misc.h"

namespace lemlib {
MotionCancelHelper::MotionCancelHelper()
    : originalCompStatus(pros::c::competition_get_status()),
      prevTime(pros::millis()) {}

bool MotionCancelHelper::wait(Time timeout) {
    // only delay if this is not the first iteration
    if (!firstIteration) pros::Task::delay_until(&prevTime, 10);
    else firstIteration = false;

    // if the competition state is not the same as when the motion started, then stop the motion
    if (pros::c::competition_get_status() != originalCompStatus) return 0;

    // check if there was a notification
    return pros::Task::notify_take(true, 0) == 0;
}
} // namespace lemlib