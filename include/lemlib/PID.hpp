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
         *
         * @b Example:
         * @code {.cpp}
         * // create a new PID controller with gains of 1, 0.1, and 0.01, a windup range of 10, and sign flip reset
         * lemlib::PID pid(1, 0.1, 0.01, 10, true);
         * @endcode
         */
        PID(double kP, double kI, double kD, double windupRange = 0, bool signFlipReset = false);
        /**
         * @brief Constructs a new PID controller
         *
         * @param gains the gains to use
         * @param windupRange range at which integral is reset
         * @param signFlipReset whether to reset integral when error changes sign
         *
         * @b Example:
         * @code {.cpp}
         * // create a new PID controller with gains of 1, 0.1, and 0.01, a windup range of 10, and sign flip reset
         * lemlib::Gains gains = {1, 0.1, 0.01};
         * lemlib::PID pid(gains, 10, true);
         * @endcode
         */
        PID(const Gains& gains, double windupRange = 0, bool signFlipReset = false);
        /**
         * @brief Get the current gains
         *
         * @return Gains the current gains
         *
         * @b Example:
         * @code {.cpp}
         * // get the currently used gains by the PID
         * lemlib::Gains gains = pid.getGains();
         * @endcode
         *
         */
        Gains getGains();
        /**
         * @brief Set the new gains
         *
         * @param gains the new gains
         *
         * @b Example:
         * @code {.cpp}
         * // create new gains
         * lemlib::Gains newGains = {1, 0.1, 0.01};
         * // set the new gains
         * pid.setGains(newGains);
         * @endcode
         */
        void setGains(Gains gains);
        /**
         * @brief Updates the PID controller using a given error, and outputs the next control signal.
         *
         * @param error the error from the setpoint. Error is calculated as setpoint - current
         * @return double the control signal (output)
         *
         * @b Example:
         * @code {.cpp}
         * // calculate current position
         * // calculate target position
         * double error = target - current;
         * // update the PID controller
         * double output = pid.update(error);
         * // move a motor based on the output
         * motor.moveVoltage(output);
         * @endcode
         */
        double update(double error);
        /**
         * @brief Resets the integral and derivative values of the PID controller.
         *
         * @b Example:
         * @code {.cpp}
         * // reset the PID controller
         * pid.reset();
         * @endcode
         */
        void reset();
        /**
         * @brief Change whether the integral is reset when the error changes sign
         *
         * @param signFlipReset whether to reset the integral when the error changes sign
         *
         * @b Example:
         * @code {.cpp}
         * // set the PID controller to reset the integral when the error changes sign
         * pid.setSignFlipReset(true);
         * @endcode
         */
        void setSignFlipReset(bool signFlipReset);
        /**
         * @brief Get the sign flip reset value
         *
         * @return true
         * @return false
         *
         * @b Example:
         * @code {.cpp}
         * // get the sign flip reset value
         * bool signFlipReset = pid.getSignFlipReset();
         * @endcode
         */
        bool getSignFlipReset();
        /**
         * @brief Set the windup range
         *
         * @param windupRange the new windup range
         *
         * @b Example:
         * @code {.cpp}
         * // set the windup range to 10
         * pid.setWindupRange(10);
         * @endcode
         */
        void setWindupRange(double windupRange);
        /**
         * @brief Get the windup range
         *
         * @return double
         *
         * @b Example:
         * @code {.cpp}
         * // get the windup range
         * double windupRange = pid.getWindupRange();
         * @endcode
         */
        double getWindupRange();
    private:
        Gains m_gains;

        bool m_signFlipReset;
        double m_windupRange;

        double m_previousError = 0;
        double m_integral = 0;

        Time m_previousTime = 0_sec;
};
} // namespace lemlib
