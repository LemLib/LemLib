// TODO: add file header and documentation

// Inspired from:
// https://theol0403.github.io/7842B-Journal/2019-10-18/task-wrapper/

#pragma once
#include <cstdio>
#include <memory>

#include "pros/rtos.hpp"

namespace lemlib {

class TaskWrapper {
    protected:
        virtual void loop() = 0;
    public:
        void start_task();
    private:
        pros::Task* task {nullptr};
};

} // namespace lemlib
