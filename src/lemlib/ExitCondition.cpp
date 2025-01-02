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

lemlib::ExitConditionGroup::ExitConditionGroup(std::vector<ExitCondition> exitConditions)
    : m_exitConditions(exitConditions) {}

bool lemlib::ExitConditionGroup::update(double input) {
    for (auto& exitCondition : m_exitConditions) {
        if (exitCondition.update(input)) return true;
    }
    return false;
}

void lemlib::ExitConditionGroup::reset() {
    for (auto& exitCondition : m_exitConditions) { exitCondition.reset(); }
}

void lemlib::ExitConditionGroup::setExitConditions(std::vector<ExitCondition> exitConditions) {
    m_exitConditions = exitConditions;
}

std::vector<lemlib::ExitCondition> lemlib::ExitConditionGroup::getExitConditions() { return m_exitConditions; }

void lemlib::ExitConditionGroup::addExitCondition(ExitCondition exitCondition) {
    m_exitConditions.push_back(exitCondition);
}
