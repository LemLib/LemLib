#include <cmath>
#include "pros/rtos.hpp"
#include "lemlib/exitcondition.hpp"

namespace lemlib {

/**
 * @brief Create a new Exit Condition
 *
 * @param range the range where the countdown is allowed to start
 * @param time how much time to wait while in range before exiting
 */
ExitCondition::ExitCondition(const float range, const int time)
    : range(range),
      time(time) {}

/**
 * @brief whether the exit condition has been met
 *
 * @return true exit condition met
 * @return false exit condition not met
 */
bool ExitCondition::getExit() { return done; }

/**
 * @brief update the exit condition
 *
 * @param input the input for the exit condition
 * @return true exit condition met
 * @return false exit condition not met
 */
bool ExitCondition::update(const float input) {
    const int curTime = pros::millis();
    if (std::fabs(input) > range) startTime = -1;
    else if (startTime == -1) startTime = curTime;
    else if (curTime >= startTime + time) done = true;
    return done;
}

/**
 * @brief reset the exit condition timer
 *
 */
void ExitCondition::reset() {
    startTime = -1;
    done = false;
}
} // namespace lemlib