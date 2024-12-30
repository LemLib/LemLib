#include "lemlib/util.hpp"
#include "lemlib/PID.hpp"

lemlib::PID::PID(double kP, double kI, double kD, double windupRange, bool signFlipReset)
    : m_gains({kP, kI, kD}),
      windupRange(windupRange),
      signFlipReset(signFlipReset) {}

lemlib::PID::PID(lemlib::Gains gains, double windupRange, bool signFlipReset)
    : m_gains(gains),
      windupRange(windupRange),
      signFlipReset(signFlipReset) {}

lemlib::Gains lemlib::PID::getGains() { return m_gains; }

void lemlib::PID::setGains(lemlib::Gains gains) { this->m_gains = gains; }

double lemlib::PID::update(double error, Time dt) {
    if (dt <= 0_sec) return 0;

    this->integral += error * dt.convert(sec);
    if (lemlib::sgn(error) != lemlib::sgn((this->previousError)) && this->signFlipReset) this->integral = 0;
    if (fabs(error) > this->windupRange && this->windupRange != 0) this->integral = 0;

    const double derivative = (error - this->previousError) / dt.convert(sec);
    this->previousError = error;

    return error * this->m_gains.kP + this->integral * this->m_gains.kI + derivative * this->m_gains.kD;
}

void lemlib::PID::reset() {
    this->previousError = 0;
    this->integral = 0;
}

void lemlib::PID::setSignFlipReset(bool signFlipReset) { this->signFlipReset = signFlipReset; }

void lemlib::PID::setWindupRange(double windupRange) { this->windupRange = windupRange; }
