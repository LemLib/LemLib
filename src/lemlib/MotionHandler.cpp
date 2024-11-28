#include "lemlib/MotionHandler.hpp"
#include "pros/rtos.hpp"
#include "pros/misc.h"

namespace lemlib::MotionHandler {
// initialize tasks
static pros::Task motionTask = pros::Task([] {});
static const pros::Task competitionCancelTask = pros::Task([] {
    int lastStatus = pros::c::competition_get_status();
    while (true) {
        const int status = pros::c::competition_get_status();
        if (status != lastStatus) cancel();
        lastStatus = status;
        pros::delay(5);
    }
});

void move(std::function<void(void)> f) {
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