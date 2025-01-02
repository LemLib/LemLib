#pragma once

#include "units/units.hpp"
#include <optional>
#include <vector>

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

class ExitConditionGroup {
    public:
        /**
         * @brief Create a new exit condition group
         *
         * @param exitConditions the exit conditions to check
         */
        ExitConditionGroup(std::vector<ExitCondition> exitConditions);
        /**
         * @brief Update the exit condition group
         *
         * @param input the input to check
         * @return true
         * @return false
         */
        bool update(double input);
        /**
         * @brief Resets the exit condition group
         *
         */
        void reset();
        /**
         * @brief Set the exit conditions in the group
         *
         * @param exitConditions the new list of exit conditions
         */
        void setExitConditions(std::vector<ExitCondition> exitConditions);
        /**
         * @brief Get the exit conditions in the group
         *
         * @return std::vector<ExitCondition> the exit conditions
         */
        std::vector<ExitCondition> getExitConditions();
        /**
         * @brief Add an exit condition to the group
         *
         * @param exitCondition the exit condition to add
         */
        void addExitCondition(ExitCondition exitCondition);
    private:
        std::vector<ExitCondition> m_exitConditions;
};
} // namespace lemlib