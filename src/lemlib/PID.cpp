#include "lemlib/util.hpp"
#include "lemlib/PID.hpp"

lemlib::PID::PID(double kP, double kI, double kD, double windupRange, bool signFlipReset)
    : m_gains({kP, kI, kD}),
      m_windupRange(windupRange),
      m_signFlipReset(signFlipReset) {}

lemlib::PID::PID(lemlib::Gains gains, double windupRange, bool signFlipReset)
    : m_gains(gains),
      m_windupRange(windupRange),
      m_signFlipReset(signFlipReset) {}

lemlib::Gains lemlib::PID::getGains() { return m_gains; }

void lemlib::PID::setGains(lemlib::Gains gains) { this->m_gains = gains; }

double lemlib::PID::update(double error, Time dt) {
    if (dt <= 0_sec) return 0;

    this->m_integral += error * dt.convert(sec);
    if (lemlib::sgn(error) != lemlib::sgn((this->m_previousError)) && this->m_signFlipReset) this->m_integral = 0;
    if (fabs(error) > this->m_windupRange && this->m_windupRange != 0) this->m_integral = 0;

    const double derivative = (error - this->m_previousError) / dt.convert(sec);
    this->m_previousError = error;

    return error * this->m_gains.kP + this->m_integral * this->m_gains.kI + derivative * this->m_gains.kD;
}

void lemlib::PID::reset() {
    this->m_previousError = 0;
    this->m_integral = 0;
}

void lemlib::PID::setSignFlipReset(bool signFlipReset) { this->m_signFlipReset = signFlipReset; }

void lemlib::PID::setWindupRange(double windupRange) { this->m_windupRange = windupRange; }
