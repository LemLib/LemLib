#pragma once

#include "units/units.hpp"
#include <optional>

namespace lemlib {
class ExitCondition {
    public:
        /**
         * @brief Create a new exit condition
         *
         * @param range how far the input can be from 0 before the timer starts
         * @param time how long the input must be within the range before the exit condition is met
         *
         * @b Example:
         * @code {.cpp}
         * // create a new exit condition that requires the input to be within 250 of 0 for 200 milliseconds
         * lemlib::ExitCondition exitCondition(250, 200_msec);
         */
        ExitCondition(double range, Time time);
        /**
         * @brief Update the exit condition
         *
         * @param input the input to check
         * @return true the exit condition has been met
         * @return false the exit condition has not been met
         *
         * @b Example:
         * @code {.cpp}
         * // update the exit condition with the input
         * if (exitCondition.update(input)) {
         *   // the exit condition has been met
         *   doSomething();
         * }
         */
        bool update(double input);
        /**
         * @brief Resets the exit condition (the timer and the done flag)
         *
         * @b Example:
         * @code {.cpp}
         * // reset the exit condition
         * exitCondition.reset();
         */
        void reset();
    private:
        std::optional<Time> m_startTime;
        bool m_done = false;
        double m_range;
        Time m_time;
};
} // namespace lemlib