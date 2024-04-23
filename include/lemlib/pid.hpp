#pragma once

#include "pros/rtos.hpp"
#include <atomic>
#include <optional>

namespace lemlib {
class PID {
    public:
        struct Gains {
                /** @brief integral gain */
                float kP;
                /** @brief proportional gain */
                float kI;
                /** @brief derivative gain */
                float kD;
                /** @brief integral anti windup range */
                float windupRange = 0;
                /** @brief whether to reset integral when sign of error flips */
                bool signFlipReset = false;
        };

        struct OptionalGains {
                /** @brief integral gain */
                std::optional<float> kP;
                /** @brief proportional gain */
                std::optional<float> kI;
                /** @brief derivative gain */
                std::optional<float> kD;
                /** @brief integral anti windup range */
                std::optional<float> windupRange = 0;
                /** @brief whether to reset integral when sign of error flips */
                std::optional<bool> signFlipReset = false;
        };

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
         * @brief Construct a new PID
         *
         * @param kP proportional gain
         * @param kI integral gain
         * @param kD derivative gain
         * @param windupRange integral anti windup range
         * @param signFlipReset whether to reset integral when sign of error flips
         */
        PID(const PID::Gains& gains);

        /**
         * @brief Update the PID
         *
         * @param error target minus position - AKA error
         * @return float output
         */
        float update(float error);

        /**
         * @brief reset integral, derivative, and prevTime
         */
        void reset();

        /**
         * @brief update the gains object while only updating the gains that are specified and ensuring atomicity
         */
        void setGains(const PID::OptionalGains& gains);
        /**
         * @brief update the gains object while and ensuring atomicity
         */
        void setGains(const PID::Gains& gains);
        /**
         * @brief get the gains object while ensuring atomicity
         */
        PID::Gains getGains() const;
    protected:
        PID::Gains gains;
        /** used to ensure the gains' atomicity */
        mutable pros::Mutex gainsMutex;

        float integral = 0;
        float prevError = 0;
};
} // namespace lemlib