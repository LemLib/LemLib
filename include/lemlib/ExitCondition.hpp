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
         */
        ExitCondition(double range, Time time);
        /**
         * @brief Update the exit condition
         *
         * @param input the input to check
         * @return true the exit condition has been met
         * @return false the exit condition has not been met
         */
        bool update(double input);
        /**
         * @brief Resets the exit condition (the timer and the done flag)
         *
         */
        void reset();
    private:
        std::optional<Time> m_startTime;
        bool m_done = false;
        double m_range;
        Time m_time;
};
} // namespace lemlib