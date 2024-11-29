#pragma once

#include <functional>

namespace lemlib::motion_handler {
/**
 * @brief run a motion algorithm
 *
 * @param f the motion function
 *
 * @b Example:
 * @code {.cpp}
 * // a simple motion algorithm, as an example
 * void simpleMotion() {
 *   std::uint32_t prevTime = pros::millis();
 *   // run until the task has been notified
 *   while (!pros::Task::notify_take(true, 0)) {
 *     // motion algorithm stuff would go here
 *     // ...
 *     // wait 10 ms to free up CPU time for other tasks
 *     pros::Task::delay_until(&prevTime, 10);
 *   }
 * }
 *
 * // runs during the autonomous period
 * void autonomous() {
 *   // pass the motion to the motion handler
 *   lemlib::motion_handler::move([&] { simpleMotion(); });
 *   // "Hello World!" is printed immediately after the motion starts
 *   std::cout << "Hello World!" << std::endl;
 *   // but if we try to run another motion while one is still running,
 *   // it will wait until the last one stops running
 *   lemlib::motion_handler::move([&] { simpleMotion(); });
 *   std::cout << "Last motion ended, new motion started!" << std::endl;
 * }
 * @endcode
 */
void move(std::function<void(void)> f);
/**
 * @brief cancel the currently running motion, if it exists
 *
 * @b Example:
 * @code {.cpp}
 * // a simple motion algorithm, as an example
 * void simpleMotion() {
 *   std::uint32_t prevTime = pros::millis();
 *   // run until the task has been notified
 *   while (!pros::Task::notify_take(true, 0)) {
 *     // motion algorithm stuff would go here
 *     // ...
 *     // wait 10 ms to free up CPU time for other tasks
 *     pros::Task::delay_until(&prevTime, 10);
 *   }
 * }
 *
 * // runs during the autonomous period
 * void autonomous() {
 *   // pass the motion to the motion handler
 *   lemlib::motion_handler::move([&] { simpleMotion(); });
 *   lemlib::motion_handler::isMoving(); // returns true
 *   // cancel the motion
 *   lemlib::motion_handler::cancel();
 *   pros::delay(10); // give the task time to stop
 *   lemlib::motion_handler::isMoving(); // returns false
 * }
 * @endcode
 */
bool isMoving();
/**
 * @brief cancel the currently running motion, if it exists
 *
 * @b Example:
 * @code {.cpp}
 * // a simple motion algorithm, as an example
 * void simpleMotion() {
 *   std::uint32_t prevTime = pros::millis();
 *   // run until the task has been notified
 *   while (!pros::Task::notify_take(true, 0)) {
 *     // motion algorithm stuff would go here
 *     // ...
 *     // wait 10 ms to free up CPU time for other tasks
 *     pros::Task::delay_until(&prevTime, 10);
 *   }
 * }
 *
 * // runs during the autonomous period
 * void autonomous() {
 *   // pass the motion to the motion handler
 *   lemlib::motion_handler::move([]{ simpleMotion(); });
 *   lemlib::motion_handler::isMoving(); // returns true
 *   // cancel the motion
 *   lemlib::motion_handler::cancel();
 *   pros::delay(10); // give the task time to stop
 *   lemlib::motion_handler::isMoving(); // returns false
 * }
 * @endcode
 */
void cancel();
} // namespace lemlib::motion_handler