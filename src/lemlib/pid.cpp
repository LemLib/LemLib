#include "pid.hpp"
#include "util.hpp"

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
    : gains(Gains {kP, kI, kD}),
      windupRange(windupRange),
      signFlipReset(signFlipReset) {}

/**
 * @brief Construct a new PID
 *
 * @param gains The PID gains
 * @param windupRange integral anti windup range
 * @param signFlipReset whether to reset integral when sign of error flips
 */
PID::PID(Gains gains, float windupRange, bool signFlipReset)
    : gains(gains),
      windupRange(windupRange),
      signFlipReset(signFlipReset) {}

/**
 * @brief Update the PID
 *
 * @param error target minus position - AKA error
 * @return float output
 */
float PID::update(const float error) {
    // calculate integral
    this->integral += error;
    if (sgn(error) != sgn((this->prevError))) this->integral = 0;
    if (fabs(error) > this->windupRange) this->integral = 0;

    // calculate derivative
    const float derivative = error - this->prevError;
    this->prevError = error;

    // calculate output
    return error * this->gains.kP + integral * this->gains.kI + derivative * this->gains.kD;
}

/**
 * @brief reset integral and error
 *
 */
void PID::reset() {
    this->integral = 0;
    this->prevError = 0;
}

Gains PID::getGains() { return this->gains; }

/**
 * @brief Set the PID gains
 *
 * @param gains The new PID gains
 */
void PID::setGains(GainOptions gains) {
    if (this->gains.kP) { this->gains.kP = *gains.kP; }

    if (this->gains.kI) { this->gains.kI = *gains.kI; }

    if (this->gains.kD) { this->gains.kD = *gains.kD; }

    this->reset();
}
} // namespace lemlib