#include "lemlib/util.hpp"
#include "lemlib/PID.hpp"
#include "pros/rtos.hpp"

lemlib::PID::PID(double kP, double kI, double kD, double windupRange, bool signFlipReset)
    : m_gains({kP, kI, kD}),
      m_windupRange(windupRange),
      m_signFlipReset(signFlipReset) {}

lemlib::PID::PID(const lemlib::Gains& gains, double windupRange, bool signFlipReset)
    : m_gains(gains),
      m_windupRange(windupRange),
      m_signFlipReset(signFlipReset) {}

lemlib::Gains lemlib::PID::getGains() { return m_gains; }

void lemlib::PID::setGains(lemlib::Gains gains) { m_gains = gains; }

double lemlib::PID::update(double error) {
    Time dt = m_previousTime == 0_sec ? 0_msec : (pros::millis() * msec) - m_previousTime;

    m_integral += error * dt.convert(sec);
    if (lemlib::sgn(error) != lemlib::sgn((m_previousError)) && m_signFlipReset) m_integral = 0;
    if (fabs(error) > m_windupRange && m_windupRange != 0) m_integral = 0;

    const double derivative = (error - m_previousError) / dt.convert(sec);
    m_previousError = error;

    return error * m_gains.kP + m_integral * m_gains.kI + derivative * m_gains.kD;
}

void lemlib::PID::reset() {
    m_previousError = 0;
    m_integral = 0;
}

void lemlib::PID::setSignFlipReset(bool signFlipReset) { m_signFlipReset = signFlipReset; }

bool lemlib::PID::getSignFlipReset() { return m_signFlipReset; }

void lemlib::PID::setWindupRange(double windupRange) { m_windupRange = windupRange; }

double lemlib::PID::getWindupRange() { return m_windupRange; }