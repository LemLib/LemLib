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
    : kP(kP),
      kI(kI),
      kD(kD),
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
    return error * this->kP + integral * this->kI + derivative * this->kD;
}

/**
 * @brief reset integral and error
 *
 */
void PID::reset() {
    this->integral = 0;
    this->prevError = 0;
}

void PID::setGains(float kP, float kI, float kD) {
    this->kP = kP;
    this->kI = kI;
    this->kD = kD;
}
} // namespace lemlib