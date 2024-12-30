#include "lemlib/ExitCondition.hpp"

#include "pros/rtos.hpp"

lemlib::ExitCondition::ExitCondition(const double range, const Time time)
    : m_startTime(std::nullopt),
      m_range(range),
      m_time(time) {}

bool lemlib::ExitCondition::update(const double input) {
    const Time currentTime = pros::millis() * msec;
    if (fabs(input) > this->m_range) this->m_startTime.reset();
    else if (this->m_startTime == -1 * sec) this->m_startTime = currentTime;
    else if (currentTime >= this->m_startTime.value() + this->m_time) this->m_done = true;

    return this->m_done;
}

void lemlib::ExitCondition::reset() {
    this->m_startTime.reset();
    this->m_done = false;
}