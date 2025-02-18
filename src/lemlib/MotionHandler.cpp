#include "lemlib/MotionHandler.hpp"
#include "pros/rtos.hpp"

namespace lemlib::motion_handler {
// initialize tasks
static std::optional<pros::Task> motionTask = std::nullopt;

void move(std::function<void(void)> f) {
    // wait until there is no motion running
    while (isMoving()) pros::delay(5);
    // start the new motion
    motionTask = pros::Task([=] {
        // only start the motion if it hasn't been cancelled yet
        if (pros::Task::notify_take(true, 0) == 0) f();
    });
}

bool isMoving() {
    // check if the task exists
    if (motionTask == std::nullopt) return false;
    // check if the task is currently running
    const std::uint32_t state = motionTask->get_state();
    return state != pros::E_TASK_STATE_DELETED &&
           state != pros::E_TASK_STATE_INVALID;
}

void cancel() {
    // if the task is currently running, notify the task
    if (isMoving()) motionTask->notify();
}
} // namespace lemlib::motion_handler
