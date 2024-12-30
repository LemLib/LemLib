#include "lemlib/ExitCondition.hpp"

#include "pros/rtos.hpp"

lemlib::ExitCondition::ExitCondition(const double range, const Time time)
    : startTime(std::nullopt),
      range(range),
      time(time) {}

bool lemlib::ExitCondition::getExit() { return this->done; }

bool lemlib::ExitCondition::update(const double input) {
    const Time currentTime = pros::millis() * msec;
    if (fabs(input) > this->range) this->startTime.reset();
    else if (this->startTime == -1 * sec) this->startTime = currentTime;
    else if (currentTime >= this->startTime.value() + this->time) this->done = true;

    return this->done;
}

void lemlib::ExitCondition::reset() {
    this->startTime.reset();
    this->done = false;
}