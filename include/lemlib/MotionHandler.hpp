#pragma once

#include <functional>
#include "pros/rtos.hpp"

namespace lemlib {
class MotionHandler {
    public:
        void move(std::function<void(void)> f);

        bool isMoving();

        void cancel();

        ~MotionHandler();
    private:
        pros::Task m_task = pros::Task([] {});
};
} // namespace lemlib