#include "pros/rtos.hpp"
#include "lemlib/timer.hpp"

namespace lemlib {
/**
 * Construct a new timer
 *
 * A timer is a useful thing, used throughout LemLib. This abstraction
 * makes the code more readable, and easier to develop.
 */
Timer::Timer(Time time)
    : period(time) {
    lastTime = pros::millis() * ms;
}

/**
 * Get the amount of time the timer is set to wait
 */
Time Timer::getTimeSet() {
    const Time time = pros::millis() * ms; // get time from RTOS
    if (!paused) timeWaited += time - lastTime; // don't update if paused
    lastTime = time; // update last time
    return period;
}

/**
 * Get the amount of time left on the timer
 */
Time Timer::getTimeLeft() {
    const Time time = pros::millis() * ms; // get time from RTOS
    if (!paused) timeWaited += time - lastTime; // don't update is paused
    lastTime = time; // update last time
    const Time delta = period - timeWaited; // calculate how much time is left
    return (delta > 0_sec) ? delta : 0_sec; // return 0 if timer is done
}

/**
 * Get the amount of time passed on the timer
 */
Time Timer::getTimePassed() {
    const Time time = pros::millis() * ms; // get time from RTOS
    if (!paused) timeWaited += time - lastTime; // don't update is paused
    lastTime = time; // update last time;
    return timeWaited;
}

/**
 * Whether the timer is done or not
 */
bool Timer::isDone() {
    const Time time = pros::millis() * ms; // get time from RTOS
    if (!paused) timeWaited += time - lastTime; // don't update is paused
    lastTime = time; // update last time
    const Time delta = period - timeWaited; // calculate how much time is left
    return delta <= 0_sec;
}

/**
 * Set how long the timer should wait. Resets the timer.
 */
void Timer::set(Time time) {
    period = time; // set how long to wait
    reset();
}

/**
 * Reset the timer
 */
void Timer::reset() {
    timeWaited = 0_sec;
    lastTime = pros::millis() * ms;
}

/**
 * Pause the timer
 */
void Timer::pause() {
    if (!paused) lastTime = pros::millis() * ms;
    paused = true;
}

/**
 * Resume the timer
 */
void Timer::resume() {
    if (paused) lastTime = pros::millis() * ms;
    paused = false;
}

/**
 * Wait until the timer is done
 */
void Timer::waitUntilDone() {
    do pros::delay(5);
    while (!this->isDone());
}
} // namespace lemlib