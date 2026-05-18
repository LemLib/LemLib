#include "lemlib/MotionHandler.hpp"
#include "LemLog/logger/Helper.hpp"
#include "pros/rtos.hpp"
#include <mutex>

namespace lemlib::motion_handler {

constexpr uint32_t NOTIFICATION_TIMEOUT = std::numeric_limits<std::uint32_t>::max();

static logger::Helper _logHelper("lemlib/motions");

static std::optional<std::function<void(void)>> _motion;
static pros::Mutex _mutex;
static uint32_t _priority = TASK_PRIORITY_DEFAULT;

// motion task
static pros::Task _motionTask([] {
    while (pros::Task::notify_take(true, NOTIFICATION_TIMEOUT)) {
        std::lock_guard lock(_mutex); // get mutex
        pros::Task::current().set_priority(_priority); // set priority back to regular value
        // run motion. _motion may legitimately be nullopt here if cancel() notified after the
        // previous motion completed naturally (benign race), so just skip in that case.
        if (_motion.has_value()) {
            try {
                _motion.value()();
            } catch (const std::exception& e) {
                _logHelper.error("motion threw an exception: {}", e.what());
            } catch (...) {
                _logHelper.error("motion threw an unknown exception");
            }
        }
        // set motion to nullopt
        _motion = std::nullopt;
    }
});

void move(std::function<void(void)> motion, std::optional<uint32_t> priority) {
    std::lock_guard lock(_mutex); // wait for any running motion to finish
    // run the motion
    _motion = motion;
    // set the priority of the task
    _priority = priority.value_or(pros::Task::current().get_priority());
    _motionTask.set_priority(TASK_PRIORITY_MAX); // temporarily set the motion task priority to max
    // notify the motion task. Since it's at MAX priority, it will preempt and start running
    // as soon as this function returns and the lock_guard releases the mutex.
    _motionTask.notify();
}

bool isMoving() {
    // if the mutex is free, no motion is running
    if (_mutex.take(0)) {
        _mutex.give();
        return false;
    }
    return true;
}

void cancel() {
    // if the task is currently running a motion, notify it so the motion can break out of its loop
    if (isMoving()) _motionTask.notify();
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
