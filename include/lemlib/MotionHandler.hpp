#pragma once

#include <functional>
#include "pros/rtos.hpp"

namespace lemlib {
/**
 * @class Motion Handler Class
 *
 * While the motion handler is not strictly necessary, it does provide some
 * quality of life improvements and prevents mistakes.
 *
 * This handler will ensure that not more than one motion can run at a time,
 * and that motions are gracefully stopped.
 */
class MotionHandler {
    public:
        /**
         * @brief run a motion algorithm
         *
         * @param f the motion function
         *
         * @code {.cpp}
         * // create a motion handler
         * lemlib::MotionHandler motionHandler;
         *
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
         *   motionHandler.move([]{ simpleMotion(); });
         *   // "Hello World!" is printed immediately after the motion starts
         *   std::cout << "Hello World!" << std::endl;
         *   // but if we try to run another motion while one is still running,
         *   // it will wait until the last one stops running
         *   motionHandler.move([]{ simpleMotion(); });
         *   std::cout << "Last motion ended, new motion started!" << std::endl;
         * }
         * @endcode
         */
        void move(std::function<void(void)> f);
        /**
         * @brief cancel the currently running motion, if it exists
         *
         * @code {.cpp}
         * // create a motion handler
         * lemlib::MotionHandler motionHandler;
         *
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
         *   motionHandler.move([]{ simpleMotion(); });
         *   motionHandler.isMoving(); // returns true
         *   // cancel the motion
         *   motionHandler.cancel();
         *   pros::delay(10); // give the task time to stop
         *   motionHandler.isMoving(); // returns false
         * }
         * @endcode
         */
        bool isMoving();
        /**
         * @brief cancel the currently running motion, if it exists
         *
         * @code {.cpp}
         * // create a motion handler
         * lemlib::MotionHandler motionHandler;
         *
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
         *   motionHandler.move([]{ simpleMotion(); });
         *   motionHandler.isMoving(); // returns true
         *   // cancel the motion
         *   motionHandler.cancel();
         *   pros::delay(10); // give the task time to stop
         *   motionHandler.isMoving(); // returns false
         * }
         * @endcode
         */
        void cancel();
        /**
         * @brief Destroy the Motion Handler object, and stop any motions
         * currently running
         */
        ~MotionHandler();
    private:
        pros::Task m_task = pros::Task([] {});
};
} // namespace lemlib