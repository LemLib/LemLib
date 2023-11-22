#pragma once

#include <cstdint>
#include "lemlib/units.hpp"

namespace lemlib {
class Timer {
    public:
        /**
         * @brief Construct a new Timer
         *
         * @param time how long to wait
         */
        Timer(Time time);
        /**
         * @brief Get the amount of time the timer was set to
         *
         * @return Time time
         */
        Time getTimeSet();
        /**
         * @brief Get the amount of time left on the timer
         *
         * @return Time time
         */
        Time getTimeLeft();
        /**
         * @brief Get the amount of time passed on the timer
         *
         * @return Time time
         */
        Time getTimePassed();
        /**
         * @brief Get whether the timer is done or not
         *
         * @return true the timer is done
         * @return false the timer is not done
         */
        bool isDone();
        /**
         * @brief Set the amount of time the timer should count down. Resets the timer
         *
         * @param time time
         */
        void set(Time time);
        /**
         * @brief reset the timer
         *
         */
        void reset();
        /**
         * @brief pause the timer
         *
         */
        void pause();
        /**
         * @brief resume the timer
         *
         */
        void resume();
        /**
         * @brief wait
         *
         */
        void waitUntilDone();
    private:
        Time period;
        Time lastTime;
        Time timeWaited = 0_sec;
        bool paused = false;
};
} // namespace lemlib