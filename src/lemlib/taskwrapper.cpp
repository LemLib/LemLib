/**
 * @file src/lemlib/taskwrapper.cpp
 * @author LemLib Team
 * @brief A Logger for LemLib.
 * @version 0.4.5
 * @date 2023-09-2
 *
 * @copyright Copyright (c) 2023
 *
 */

// PROS requires that the addresses of callbacks passed into tasks must be known at compile time. To get around this we
// use a lambda function, and call the function we want to run inside it. For more information look here:
// https://theol0403.github.io/7842B-Journal/2019-10-18/task-wrapper/

#include "lemlib/taskwrapper.hpp"

namespace lemlib {
void TaskWrapper::start_task() {
    if (task == nullptr) {
        task = new pros::Task([this] { this->loop(); });
    }
}
}; // namespace lemlib
