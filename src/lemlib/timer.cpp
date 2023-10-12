#include "pros/rtos.hpp"
#include "lemlib/timer.hpp"

using namespace lemlib;

/**
 * Construct a new timer
 *
 * A timer is a useful thing, used throughout LemLib. This abstraction
 * makes the code more readable, and easier to develop.
 */
Timer::Timer(uint32_t time) : period(time) { lastTime = pros::millis(); }

/**
 * Get the amount of time the timer is set to wait
 */
uint32_t Timer::getTimeSet() {
    const uint32_t time = pros::millis(); // get time from RTOS
    if (!paused) timeWaited += time - lastTime; // don't update if paused
    lastTime = time; // update last time
    return period;
}

/**
 * Get the amount of time left on the timer
 */
uint32_t Timer::getTimeLeft() {
    const uint32_t time = pros::millis(); // get time from RTOS
    if (!paused) timeWaited += time - lastTime; // don't update is paused
    lastTime = time; // update last time
    const int delta = period - timeWaited; // calculate how much time is left
    return (delta > 0) ? delta : 0; // return 0 if timer is done
}

/**
 * Get the amount of time passed on the timer
 */
uint32_t Timer::getTimePassed() {
    const uint32_t time = pros::millis(); // get time from RTOS
    if (!paused) timeWaited += time - lastTime; // don't update is paused
    lastTime = time; // update last time;
    return timeWaited;
}

/**
 * Whether the timer is done or not
 */
bool Timer::isDone() {
    const uint32_t time = pros::millis(); // get time from RTOS
    if (!paused) timeWaited += time - lastTime; // don't update is paused
    lastTime = time; // update last time
    const int delta = period - timeWaited; // calculate how much time is left
    return delta <= 0;
}

/**
 * Set how long the timer should wait. Resets the timer.
 */
void Timer::set(uint32_t time) {
    period = time; // set how long to wait
    reset();
}

/**
 * Reset the timer
 */
void Timer::reset() {
    timeWaited = 0;
    lastTime = pros::millis();
}

/**
 * Pause the timer
 */
void Timer::pause() {
    if (!paused) lastTime = pros::millis();
    paused = true;
}

/**
 * Resume the timer
 */
void Timer::resume() {
    if (paused) lastTime = pros::millis();
    paused = false;
}

/**
 * Wait until the timer is done
 */
void Timer::waitUntilDone() {
    do pros::delay(5);
    while (!this->isDone());
}
