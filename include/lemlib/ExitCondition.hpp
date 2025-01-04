#pragma once

#include "units/units.hpp"
#include "pros/rtos.hpp"
#include <optional>
#include <vector>

namespace lemlib {
template <isQuantity Q> class ExitCondition {
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
        ExitCondition(Q range, Time time)
            : m_startTime(std::nullopt),
              m_range(range),
              m_time(time) {}

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
        bool update(Q input) {
            const Time currentTime = pros::millis() * msec;
            if (units::abs(input) >= m_range) m_startTime.reset();
            else if (m_startTime == -1 * sec) m_startTime = currentTime;
            else if (currentTime >= m_startTime.value() + m_time) m_done = true;

            return m_done;
        }

        /**
         * @brief Resets the exit condition (the timer and the done flag)
         *
         * @b Example:
         * @code {.cpp}
         * // reset the exit condition
         * exitCondition.reset();
         */
        void reset() {
            m_startTime.reset();
            m_done = false;
        }
    private:
        std::optional<Time> m_startTime;
        bool m_done = false;
        Q m_range;
        Time m_time;
};

template <isQuantity Q> class ExitConditionGroup {
    public:
        /**
         * @brief Create a new exit condition group
         *
         * @param exitConditions the exit conditions to check
         */
        ExitConditionGroup(std::vector<ExitCondition<Q>> exitConditions)
            : m_exitConditions(exitConditions) {}

        /**
         * @brief Update the exit condition group
         *
         * @param input the input to check
         * @return true
         * @return false
         */
        bool update(Q input) {
            for (auto& exitCondition : m_exitConditions) {
                if (exitCondition.update(input)) return true;
            }
            return false;
        }

        /**
         * @brief Resets the exit condition group
         *
         */
        void reset() {
            for (auto& exitCondition : m_exitConditions) { exitCondition.reset(); }
        }

        /**
         * @brief Set the exit conditions in the group
         *
         * @param exitConditions the new list of exit conditions
         */
        void setExitConditions(std::vector<ExitCondition<Q>> exitConditions) { m_exitConditions = exitConditions; }

        /**
         * @brief Get the exit conditions in the group
         *
         * @return std::vector<ExitCondition> the exit conditions
         */
        std::vector<ExitCondition<Q>> getExitConditions() { return m_exitConditions; }

        /**
         * @brief Add an exit condition to the group
         *
         * @param exitCondition the exit condition to add
         */
        void addExitCondition(ExitCondition<Q> exitCondition) { m_exitConditions.push_back(exitCondition); }
    private:
        std::vector<ExitCondition<Q>> m_exitConditions;
};
} // namespace lemlib