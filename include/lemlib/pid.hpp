#pragma once
#include <optional>

namespace lemlib {
struct GainOptions {
        std::optional<float> kP = std::nullopt;
        std::optional<float> kI = std::nullopt;
        std::optional<float> kD = std::nullopt;
};

struct Gains {
        float kP;
        float kI;
        float kD;
        Gains() = delete;
};

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

        PID(Gains gains, float winupRange = 0, bool signFlipReset = false);
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

        /**
         * @brief Get the PID gains
         *
         * @return the current PID gains
         */
        Gains getGains();

        /**
         * @brief Set the PID gains
         *
         * @param gains the new PID gains
         */
        void setGains(GainOptions gains);
    protected:
        // gains
        Gains gains;

        // optimizations
        const float windupRange;
        const bool signFlipReset;

        float integral = 0;
        float prevError = 0;
};
} // namespace lemlib