#pragma once

#include "units/units.hpp"

namespace lemlib {
/**
 * @brief Struct to hold PID gains.
 *
 * @param kP proportional gain
 * @param kI integral gain
 * @param kD derivative gain
 */
struct Gains {
        double kP = 0;
        double kI = 0;
        double kD = 0;
};

class PID {
    public:
        /**
         * @brief Constructs a new PID controller
         *
         * @param kP proportional gain
         * @param kI integral gain
         * @param kD derivative gain
         * @param windupRange range at which integral is reset
         * @param signFlipReset whether to reset integral when error changes sign
         */
        PID(double kP, double kI, double kD, double windupRange = 0, bool signFlipReset = false);
        /**
         * @brief Constructs a new PID controller
         *
         * @param gains the gains to use
         * @param windupRange range at which integral is reset
         * @param signFlipReset whether to reset integral when error changes sign
         */
        PID(const Gains& gains, double windupRange = 0, bool signFlipReset = false);
        /**
         * @brief Get the current gains
         *
         * @return Gains the current gains
         */
        Gains getGains();
        /**
         * @brief Set the new gains
         *
         * @param gains the new gains
         */
        void setGains(Gains gains);
        /**
         * @brief Updates the PID controller using a given error, and outputs the next control signal.
         *
         * @param error the error from the setpoint. Error is calculated as setpoint - current
         * @return double the control signal (output)
         */
        double update(double error, Time dt);
        /**
         * @brief Resets the integral and derivative values of the PID controller.
         *
         */
        void reset();
        /**
         * @brief Change whether the integral is reset when the error changes sign
         *
         * @param signFlipReset whether to reset the integral when the error changes sign
         */
        void setSignFlipReset(bool signFlipReset);
        /**
         * @brief Get the sign flip reset value
         *
         * @return true
         * @return false
         */
        bool getSignFlipReset();
        /**
         * @brief Set the windup range
         *
         * @param windupRange the new windup range
         */
        void setWindupRange(double windupRange);
        /**
         * @brief Get the windup range
         *
         * @return double
         */
        double getWindupRange();
    private:
        Gains m_gains;

        bool m_signFlipReset;
        double m_windupRange;

        double m_previousError = 0;
        double m_integral = 0;
};
} // namespace lemlib
