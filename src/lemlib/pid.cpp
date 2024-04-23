#include "pid.hpp"
#include "util.hpp"
#include <mutex>

namespace lemlib {
/**
 * @brief Construct a new PID
 *
 * @param kP proportional gain
 * @param kI integral gain
 * @param kD derivative gain
 * @param windupRange integral anti windup range
 * @param signFlipReset whether to reset integral when sign of error flips
 */
PID::PID(float kP, float kI, float kD, float windupRange, bool signFlipReset)
    : PID(PID::Gains {.kP = kP, .kI = kI, .kD = kD, .windupRange = windupRange, .signFlipReset = signFlipReset}) {}

PID::PID(const PID::Gains& gains)
    : gains(gains) {}

/**
 * @brief Update the PID
 *
 * @param error target minus position - AKA error
 * @return float output
 */
float PID::update(const float error) {
    const PID::Gains gains = this->getGains();
    // calculate integral
    integral += error;
    if (sgn(error) != sgn((prevError)) && gains.signFlipReset) integral = 0;
    if (fabs(error) > gains.windupRange && gains.windupRange != 0) integral = 0;

    // calculate derivative
    const float derivative = error - prevError;
    prevError = error;

    // calculate output
    return error * gains.kP + integral * gains.kI + derivative * gains.kD;
}

void PID::setGains(const PID::OptionalGains& newGains) {
    const PID::Gains oldGains = this->getGains();
    this->setGains(PID::Gains {.kP = newGains.kP.value_or(oldGains.kP),
                               .kI = newGains.kI.value_or(oldGains.kI),
                               .kD = newGains.kD.value_or(oldGains.kD),
                               .windupRange = newGains.windupRange.value_or(oldGains.windupRange),
                               .signFlipReset = newGains.signFlipReset.value_or(oldGains.signFlipReset)});
}

void PID::setGains(const PID::Gains& newGains) {
    std::lock_guard guard(this->gainsMutex);
    this->gains = newGains;
}

PID::Gains PID::getGains() const {
    std::lock_guard guard(this->gainsMutex);
    return this->gains;
}

/**
 * @brief reset integral and error
 *
 */
void PID::reset() {
    integral = 0;
    prevError = 0;
}

PID::OptionalGains PID::OptionalGains::fromGains(const PID::Gains& gains) {
    return OptionalGains {.kP = gains.kP,
                          .kI = gains.kI,
                          .kD = gains.kD,
                          .windupRange = gains.windupRange,
                          .signFlipReset = gains.signFlipReset};
}
} // namespace lemlib