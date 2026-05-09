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
    std::lock_guard lock(mutex); // wait for any running motion to finish
    // run the motion
    motion = _motion;
    // set the priority of the task
    priority = _priority ? *_priority : pros::Task::current().get_priority();
    motionTask.set_priority(TASK_PRIORITY_MAX); // temporarily set the motion task priority to max
    motionTask.notify(); // tell the motion handler it can run
    pros::delay(1); // force context switch, so the motion starts running immediately
}

bool isMoving() {
    // if the mutex is free, no motion is running
    if (mutex.take(0)) {
        mutex.give();
        return false;
    }
    return true;
}

void cancel() {
    // if the task is currently running a motion, notify it so the motion can break out of its loop
    if (isMoving()) motionTask.notify();
}

void waitUntilPoint(units::V2Position target, Length radius, std::function<units::Pose()> poseGetter) {
    do {
        const units::Pose pose = poseGetter();
        if (pose.distanceTo(target) <= radius) return;
        pros::delay(5);
    } while (isMoving());
}

void waitUntilDistance(Length dist, std::function<units::Pose()> poseGetter) {
    const units::V2Position start = poseGetter();
    do {
        const units::Pose pose = poseGetter();
        if (pose.distanceTo(start) >= dist) return;
        pros::delay(5);
    } while (isMoving());
}
} // namespace lemlib::motion_handler
