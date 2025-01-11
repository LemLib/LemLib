#pragma once

#include "units/units.hpp"
#include "pros/rtos.hpp"

namespace lemlib {
class Timer {
    public:
        /**
         * @brief Construct a new Timer
         *
         * @note the timer will start counting down as soon as it is created
         * @note the timer constructor is non-blocking so code after it will be executed immediately
         * @note if the timer is constructed in a global scope, its behavior is undefined. You can
         *       call set() before using the timer if you absolutely need to construct it in a global scope
         *
         * @param time how long to wait
         *
         * @b Example
         * @code {.cpp}
         * // create a timer that will wait for 1 second
         * Timer timer(1_sec);
         * @endcode
         */
        Timer(Time time);
        /**
         * @brief Get the amount of time the timer was set to
         *
         * @return Time time
         *
         * @b Example
         * @code {.cpp}
         * // create a timer that will wait for 1 second
         * Timer timer(1_sec);
         * // get the time the timer was set to
         * const Time time = timer.getTimeSet(); // time = 1_sec
         * @endcode
         */
        Time getTimeSet();
        /**
         * @brief Get the amount of time left on the timer
         *
         * @return Time time
         *
         * @b Example
         * @code {.cpp}
         * // create a timer that will wait for 1 second
         * Timer timer(1_sec);
         * // delay for 300ms
         * pros::delay(300);
         * // get the time left on the timer
         * const Time time = timer.getTimeLeft(); // time = 700_msec
         * @endcode
         */
        Time getTimeLeft();
        /**
         * @brief Get the amount of time passed on the timer
         *
         * @return Time time
         *
         * @b Example
         * @code {.cpp}
         * // create a timer that will wait for 1 second
         * Timer timer(1_sec);
         * // delay for 300ms
         * pros::delay(300);
         * // get the time passed on the timer
         * const Time time = timer.getTimePassed(); // time = 300_msec
         * @endcode
         */
        Time getTimePassed();
        /**
         * @brief Get whether the timer is done or not
         *
         * @return true the timer is done
         * @return false the timer is not done
         *
         * @b Example
         * @code {.cpp}
         * // create a timer that will wait for 1 second
         * Timer timer(1_sec);
         * // delay for 500ms
         * pros::delay(500);
         * // check if the timer is done
         * const bool done = timer.isDone(); // done = false
         * // delay for another 500ms
         * pros::delay(500);
         * // check if the timer is done
         * const bool done = timer.isDone(); // done = true
         * @endcode
         */
        bool isDone();
        /**
         * @brief Get whether the timer is paused or not
         *
         * @return true the timer is paused
         * @return false the timer is not paused
         *
         * @b Example
         * @code {.cpp}
         * // create a timer that will wait for 1 second
         * Timer timer(1_sec);
         * // pause the timer
         * timer.pause();
         * // check if the timer is paused
         * bool paused = timer.isPaused(); // paused = true
         * // resume the timer
         * timer.resume();
         * // check if the timer is paused
         * paused = timer.isPaused(); // paused = false
         * @endcode
         */
        bool isPaused();
        /**
         * @brief Set the amount of time the timer should count down. Resets the timer
         *
         * @param time time in milliseconds
         *
         * @b Example
         * @code {.cpp}
         * // create a timer that will wait for 1 second
         * Timer timer(1_sec);
         * // set the timer to wait for 2 seconds
         * timer.set(2_sec);
         * @endcode
         */
        void set(Time time);
        /**
         * @brief reset the timer
         *
         * @b Example
         * @code {.cpp}
         * // create a timer that will wait for 1 second
         * Timer timer(1_sec);
         * // delay for 500ms
         * pros::delay(500);
         * // reset the timer
         * timer.reset();
         * // delay for another 500ms
         * pros::delay(500);
         * // check if the timer is done
         * const bool done = timer.isDone(); // done = false
         * @endcode
         */
        void reset();
        /**
         * @brief pause the timer
         *
         * @b Example
         * @code {.cpp}
         * // create a timer that will wait for 1 second
         * Timer timer(1_sec);
         * // pause the timer
         * timer.pause();
         * // delay for 2000ms
         * pros::delay(2000);
         * // check if the timer is done
         * const bool done = timer.isDone(); // done = false
         * @endcode
         */
        void pause();
        /**
         * @brief resume the timer
         *
         * @b Example
         * @code {.cpp}
         * // create a timer that will wait for 1 second
         * Timer timer(1_sec);
         * // pause the timer
         * timer.pause();
         * // delay for 500ms
         * pros::delay(500);
         * // resume the timer
         * timer.resume();
         * // delay for another 500ms
         * pros::delay(500);
         * // check if the timer is done
         * const bool done = timer.isDone(); // done = false
         * @endcode
         */
        void resume();
        /**
         * @brief wait until the timer is done
         *
         * @b Example
         * @code {.cpp}
         * // create a timer that will wait for 1 second
         * Timer timer(1_sec);
         * // wait until the timer is done
         * timer.waitUntilDone();
         * std::cout << "done!" << std::endl;
         * @endcode
         */
        void waitUntilDone();
    private:
        void update();

        pros::Mutex m_mutex;
        Time m_period;
        Time m_lastTime = 0_sec;
        Time m_timeWaited = 0_sec;
        bool m_paused = false;
};
} // namespace lemlib