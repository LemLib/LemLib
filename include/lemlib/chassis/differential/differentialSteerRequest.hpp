#pragma once

#include "pros/motor_group.hpp"
#include <memory>

namespace lemlib {
class DifferentialSteerRequest {
    public:
        virtual void apply(std::shared_ptr<pros::MotorGroup> left, std::shared_ptr<pros::MotorGroup> right) = 0;
};
} // namespace lemlib
