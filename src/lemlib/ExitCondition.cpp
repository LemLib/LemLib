#include "lemlib/ExitCondition.hpp"

#include "pros/rtos.hpp"

lemlib::ExitCondition::ExitCondition(double range, Time time)
    : m_startTime(std::nullopt),
      m_range(range),
      m_time(time) {}

bool lemlib::ExitCondition::update(double input) {
    const Time currentTime = pros::millis() * msec;
    if (fabs(input) >= m_range) m_startTime.reset();
    else if (m_startTime == -1 * sec) m_startTime = currentTime;
    else if (currentTime >= m_startTime.value() + m_time) m_done = true;

    return m_done;
}

void lemlib::ExitCondition::reset() {
    m_startTime.reset();
    m_done = false;
}