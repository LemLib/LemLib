#include "lemlib/MotionHandler.hpp"
#include "pros/rtos.hpp"
#include "pros/misc.h"

namespace lemlib::MotionHandler {
// initialize tasks
static pros::Task motionTask = pros::Task([] {});
static int lastStatus = -1;
static const pros::Task competitionCancelTask = pros::Task([] {
    while (true) {
        const int status = pros::c::competition_get_status();
        if (status != lastStatus) cancel();
        lastStatus = status;
        pros::delay(5);
    }
});

void move(std::function<void(void)> f) {
    // prevent an edge case where the motion can be cancelled
    // if its started immediately after the competition state changes
    const int status = pros::c::competition_get_status();
    if (status != lastStatus) cancel();
    lastStatus = status;
    // wait until there is no motion running
    while (isMoving()) pros::delay(5);
    // start the new motion
    motionTask = pros::Task(f);
}

bool isMoving() {
    // check if the task is currently running
    const std::uint32_t state = motionTask.get_state();
    return state != pros::E_TASK_STATE_DELETED && state != pros::E_TASK_STATE_INVALID;
}

void cancel() {
    const std::uint32_t state = motionTask.get_state();
    // if the task is currently running, notify the task
    if (state == pros::E_TASK_STATE_DELETED || state == pros::E_TASK_STATE_INVALID) motionTask.notify();
}
} // namespace lemlib::MotionHandler