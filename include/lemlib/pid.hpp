#pragma once

namespace lemlib {
class PID {
    public:
        /**
         * @brief Construct a new PID
         *
         * @param kP proportional gain
         * @param kI integral gain
         * @param kD derivative gain
         * @param windupRange integral anti windup range
         * @param signFlipReset whether to reset integral when sign of error flips
         */
        PID(float kP, float kI, float kD, float windupRange = 0, bool signFlipReset = false);

        /**
         * @brief Update the PID
         *
         * @param error target minus position - AKA error
         * @return float output
         */
        float update(float error);

        /**
         * @brief reset integral, derivative, and prevTime
         *
         */
        void reset();
        // gains
        float kP;
        float kI;
        float kD;
    protected:
        // optimizations
        const float windupRange;
        const bool signFlipReset;

        float integral = 0;
        float prevError = 0;
};
} // namespace lemlib