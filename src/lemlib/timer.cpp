#include "pros/rtos.hpp"
#include "lemlib/timer.hpp"

using namespace lemlib;

Timer::Timer(uint32_t time)
    : period(time) {
    lastTime = pros::millis();
}

uint32_t Timer::getTimeSet() {
    const uint32_t time = pros::millis(); // get time from RTOS
    if (!paused) timeWaited += time - lastTime; // don't update if paused
    lastTime = time; // update last time
    return period;
}

uint32_t Timer::getTimeLeft() {
    const uint32_t time = pros::millis(); // get time from RTOS
    if (!paused) timeWaited += time - lastTime; // don't update if paused
    lastTime = time; // update last time
    const int delta = period - timeWaited; // calculate how much time is left
    return (delta > 0) ? delta : 0; // return 0 if timer is done
}

uint32_t Timer::getTimePassed() {
    const uint32_t time = pros::millis(); // get time from RTOS
    if (!paused) timeWaited += time - lastTime; // don't update if paused
    lastTime = time; // update last time;
    return timeWaited;
}

bool Timer::isDone() {
    const uint32_t time = pros::millis(); // get time from RTOS
    if (!paused) timeWaited += time - lastTime; // don't update if paused
    lastTime = time; // update last time
    const int delta = period - timeWaited; // calculate how much time is left
    return delta <= 0;
}

bool Timer::isPaused() {
    const uint32_t time = pros::millis(); // get time from RTOS
    if (!paused) timeWaited += time - lastTime; // don't update if paused
    return paused;
}

void Timer::set(uint32_t time) {
    period = time; // set how long to wait
    reset();
}

void Timer::reset() {
    timeWaited = 0;
    lastTime = pros::millis();
}

void Timer::pause() {
    if (!paused) lastTime = pros::millis();
    paused = true;
}

void Timer::resume() {
    if (paused) lastTime = pros::millis();
    paused = false;
}

void Timer::waitUntilDone() {
    do pros::delay(5);
    while (!this->isDone());
}
