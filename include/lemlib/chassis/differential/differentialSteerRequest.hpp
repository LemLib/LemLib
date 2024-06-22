#pragma once

#include "pros/motor_group.hpp"
#include <memory>

namespace lemlib {
/**
 * @class DifferentialSteerRequest
 *
 * @brief Abstract class used to control motors of a differential drive robot
 *
 * An instance of a child class of DifferentialMotion returns a pointer to an instance of a child class of this class.
 * This is necessary because motions may want to control the motors in different ways. For example, a motion may want to
 * control the voltage of the motors directly, while another motion may want to control the velocity of the motors. This
 * needs to be abstracted so motions don't have to worry about how the motors are controlled, or the motion handler.
 *
 * This may seem like a somewhat unnecessary abstraction: "why not just pass the motor group to the motion constructor?"
 * Because some of these control methods can be complex, and we also do not want to expose the motor group to the
 * motion, as this could lead to the motion handler being bypassed.
 */
class DifferentialSteerRequest {
    public:
        /**
         * @brief apply the request to the motors
         *
         * @param left shared pointer to the left motor group
         * @param right shared pointer to the right motor group
         */
        virtual void apply(std::shared_ptr<pros::MotorGroup> left, std::shared_ptr<pros::MotorGroup> right) = 0;
        /**
         * @brief Destroy the Differential Steer Request object
         *
         */
        virtual ~DifferentialSteerRequest();
};
} // namespace lemlib
