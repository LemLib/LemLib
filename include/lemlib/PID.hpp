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
        Number kP = 0;
        Number kI = 0;
        Number kD = 0;
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
         * // create a new PID controller with gains of 2, 0.5, and 0.1, with no windup range or sign flip reset
         * lemlib::PID pid2(2, 0.5, 0.1);
         * @endcode
         */
        PID(Number kP, Number kI, Number kD, Number windupRange = 0, bool signFlipReset = false);
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
         * // create a new PID controller with gains of 2, 0.5, and 0.1, with no windup range or sign flip reset
         * lemlib::Gains gains2 = {2, 0.5, 0.1};
         * lemlib::PID pid2(gains2);
         * @endcode
         */
        PID(const Gains& gains, Number windupRange = 0, bool signFlipReset = false);
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
         * @return Number the control signal (output)
         *
         * @b Example:
         * @code {.cpp}
         * // calculate current position
         * // calculate target position
         * Number error = target - current;
         * // update the PID controller
         * Number output = pid.update(error);
         * // move a motor based on the output
         * motor.moveVoltage(output);
         * @endcode
         */
        Number update(Number error);
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
        void setWindupRange(Number windupRange);
        /**
         * @brief Get the windup range
         *
         * @return Number
         *
         * @b Example:
         * @code {.cpp}
         * // get the windup range
         * Number windupRange = pid.getWindupRange();
         * @endcode
         */
        Number getWindupRange();
    private:
        Gains m_gains;

        bool m_signFlipReset;
        Number m_windupRange;

        Number m_previousError = 0;
        Number m_integral = 0;

        std::optional<Time> m_previousTime = std::nullopt;
};
} // namespace lemlib
