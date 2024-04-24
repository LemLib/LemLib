#pragma once

namespace lemlib {
class ExitCondition {
    public:
        /**
         * @brief Create a new Exit Condition
         *
         * @param range the range where the countdown is allowed to start
         * @param time how much time to wait while in range before exiting
         *
         * @b Example
         * @code {.cpp}
         * // create a new exit condition that will exit if the input is within 0.1 of the target for 1000ms
         * ExitCondition ec(0.1, 1000);
         * @endcode
         */
        ExitCondition(const float range, const int time);
        /**
         * @brief whether the exit condition has been met
         *
         * @return true exit condition met
         * @return false exit condition not met
         *
         * @b Example
         * @code {.cpp}
         * // check if the exit condition has been met
         * if (ec.getExit()) {
         *     // do something
         * }
         * @endcode
         */
        bool getExit();
        /**
         * @brief update the exit condition
         *
         * @param input the input for the exit condition
         * @return true exit condition met
         * @return false exit condition not met
         *
         * @b Example
         * @code {.cpp}
         * // update the exit condition
         * // this is typically called in a loop
         * while (!ec.getExit()) {
         *     // do something
         *     ec.update(input);
         * }
         * @endcode
         */
        bool update(const float input);
        /**
         * @brief reset the exit condition timer
         *
         * @b Example
         * @code {.cpp}
         * // reset the exit condition timer
         * ec.reset();
         * @endcode
         */
        void reset();
    protected:
        const float range;
        const int time;
        int startTime = -1;
        bool done = false;
};
} // namespace lemlib