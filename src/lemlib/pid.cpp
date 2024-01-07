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
    integral += error;
    if (sgn(error) != sgn((prevError))) integral = 0;
    if (fabs(error) > windupRange) integral = 0;

    // calculate derivative
    const float derivative = error - prevError;
    prevError = error;

    // calculate output
    return error * kP + integral * kI + derivative * kD;
}

/**
 * @brief reset integral and error
 *
 */
void PID::reset() {
    integral = 0;
    prevError = 0;
}
} // namespace lemlib