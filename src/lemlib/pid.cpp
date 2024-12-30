#include "lemlib/util.hpp"
#include "lemlib/pid.hpp"

#include "pros/rtos.hpp"

lemlib::PID::PID(double kP, double kI, double kD, double windupRange, bool signFlipReset)
    : gains({kP, kI, kD}),
      windupRange(windupRange),
      signFlipReset(signFlipReset) {}

lemlib::PID::PID(lemlib::Gains gains, double windupRange, bool signFlipReset)
    : gains(gains),
      windupRange(windupRange),
      signFlipReset(signFlipReset) {}

lemlib::Gains lemlib::PID::getGains() { return gains; }

void lemlib::PID::setGains(lemlib::Gains gains) { this->gains = gains; }

double lemlib::PID::update(double error) {
    this->integral += error;
    if (lemlib::sgn(error) != lemlib::sgn((this->previousError)) && this->signFlipReset) this->integral = 0;
    if (fabs(error) > this->windupRange && this->windupRange != 0) this->integral = 0;

    const double derivative = error - this->previousError;
    this->previousError = error;

    return error * this->gains.kP + this->integral * this->gains.kI + derivative * this->gains.kD;
}

void lemlib::PID::reset() {
    this->previousError = 0;
    this->integral = 0;
}

lemlib::ExitCondition::ExitCondition(const double range, const Time time)
    : range(range),
      time(time) {}

bool lemlib::ExitCondition::getExit() { return this->done; }

bool lemlib::ExitCondition::update(const double input) {
    const Time currentTime = pros::millis() * msec;
    if (fabs(input) > this->range) this->startTime = -1 * sec;
    else if (this->startTime == -1 * sec) this->startTime = currentTime;
    else if (currentTime >= this->startTime + this->time) this->done = true;

    return this->done;
}

void lemlib::ExitCondition::reset() {
    this->startTime = -1 * sec;
    this->done = false;
}