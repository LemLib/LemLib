#include "lemlib/MotionHandler.hpp"
#include "pros/rtos.h"

namespace lemlib {
void MotionHandler::move(std::function<void(void)> f) {
    // wait until there is no motion running
    while (this->isMoving()) pros::delay(5);
    // start the new motion
    m_task = pros::Task(f);
}

bool MotionHandler::isMoving() {
    // check if the task is currently running
    const std::uint32_t state = m_task.get_state();
    return state != pros::E_TASK_STATE_DELETED && state != pros::E_TASK_STATE_INVALID;
}

void MotionHandler::cancel() {
    const std::uint32_t state = m_task.get_state();
    // if the task is currently running, notify the task
    if (state == pros::E_TASK_STATE_DELETED || state == pros::E_TASK_STATE_INVALID) m_task.notify();
}

MotionHandler::~MotionHandler() {
    cancel(); // stop the current motion if the handler is deconstructed
}
} // namespace lemlib