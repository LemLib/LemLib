#include "lemlib/MotionHandler.hpp"
#include "LemLog/logger/Helper.hpp"
#include "pros/rtos.hpp"
#include <mutex>

namespace lemlib::motion_handler {

constexpr uint32_t NOTIFICATION_TIMEOUT = std::numeric_limits<std::uint32_t>::max();

static logger::Helper logHelper("lemlib/motions");

static std::optional<std::function<void(void)>> motion;
static pros::Mutex mutex;
static uint32_t priority = TASK_PRIORITY_DEFAULT;

// motion task
static pros::Task motionTask([] {
    while (pros::Task::notify_take(true, NOTIFICATION_TIMEOUT)) {
        std::lock_guard lock(mutex); // get mutex
        pros::Task::current().set_priority(priority); // set priority back to regular value
        // run motion
        if (motion) (*motion)();
        else logHelper.error("motion task notified, but no motion to run! This is a bug and should be reported");
        // set motion to nullopt
        motion = std::nullopt;
    }
});

void move(std::function<void(void)> _motion, std::optional<uint32_t> _priority) {
    // check for a very improbable edge case
    if (mutex.take(0) && !motion) pros::delay(10);
    if (mutex.take(0) && !motion) {
        logHelper.error("mutex error! This is a bug and should be reported! Skipping queued motion");
        return;
    }
    std::lock_guard lock(mutex); // wait for the motion to finish
    // run the motion
    motion = _motion;
    // set the priority of the task
    priority = _priority ? *_priority : pros::Task::current().get_priority();
    motionTask.set_priority(TASK_PRIORITY_MAX); // temporarily set the motion task priority to max
    motionTask.notify(); // tell the motion handler it can run
    pros::delay(1); // force context switch, so the motion starts running immediately
}

void cancel() {}
} // namespace lemlib::motion_handler