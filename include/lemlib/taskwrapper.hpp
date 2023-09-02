/**
 * @file include/lemlib/taskwrapper.hpp
 * @author LemLib Team
 * @brief A Logger for LemLib.
 * @version 0.4.5
 * @date 2023-09-2
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include <cstdio>
#include <memory>

#include "pros/rtos.hpp"

namespace lemlib {

/**
 * @brief An abstract class meant for long running tasks.
 */

class TaskWrapper {
    protected:
        /**
         * @brief The function that will be run inside the task.
         */
        virtual void loop() = 0;
    public:
        /**
         * @brief The function that will be run inside the task. Starts the task.
         * @note This will not call a while loop or any delay, so those will need to be provided inside the loop method
         * if that behaviour is desired.
         */
        void start_task();
    private:
        pros::Task* task {nullptr};
};

} // namespace lemlib
