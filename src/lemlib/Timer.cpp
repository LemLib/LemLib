#include "lemlib/Timer.hpp"
#include "pros/rtos.hpp"

using namespace lemlib;

Timer::Timer(Time time)
    : m_period(time) {
    m_lastTime = pros::millis() * msec;
}

void Timer::update() {
    const Time time = pros::millis() * msec; // get current time from RTOS
    if (!m_paused) m_timeWaited += time - m_lastTime; // dont update if paused
    m_lastTime = time; // update last time
}

Time Timer::getTimeSet() {
    this->update();
    return m_period;
}

Time Timer::getTimeLeft() {
    this->update();
    const Time delta =
      m_period - m_timeWaited; // calculate how much time is left
    return (delta > 0_sec) ? delta : 0_sec; // return 0 if timer is done
}

Time Timer::getTimePassed() {
    this->update();
    return m_timeWaited;
}

bool Timer::isDone() {
    this->update();
    Time delta = m_period - m_timeWaited; // calculate how much time is left
    return delta <= 0_sec;
}

bool Timer::isPaused() {
    const Time time = pros::millis() * msec; // get time from RTOS
    if (!m_paused) m_timeWaited += time - m_lastTime; // dont update if paused
    return m_paused;
}

void Timer::set(Time time) {
    m_period = time; // set how long the timer should run
    this->reset();
}

void Timer::reset() {
    m_timeWaited = 0_sec;
    m_lastTime = pros::millis() * msec;
}

void Timer::pause() {
    if (!m_paused) m_lastTime = pros::millis() * msec;
    m_paused = true;
}

void Timer::resume() {
    if (m_paused) m_lastTime = pros::millis() * msec;
    m_paused = false;
}

void Timer::waitUntilDone() {
    do pros::delay(5);
    while (!this->isDone());
}
