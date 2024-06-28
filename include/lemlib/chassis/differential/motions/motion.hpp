#pragma once

#include "lemlib/chassis/differential/drivetrain.hpp"

namespace lemlib::differential {
/**
 * @brief abstract motion class
 *
 * This is an abstract motion class for a differential drivetrain. It can be used to create
 * custom motions for the chassis, implemented as part of LemLib, part of an extension for
 * LemLib, or a user made extension specific to a single project.
 *
 * @b Example Child Class
 * @code {.cpp}
 * class SimpleForward : public Motion {
 *   public:
 *     SimpleForward() {}
 *
 *     void calculate(Drivetrain& drivetrain, const ChassisState& state) override {
 *       drivetrain.left.move(127);
 *       drivetrain.right.move(127);
 *     }
 *
 *     bool isFinished() override {
 *       return false; // this motion never finishes
 *     }
 * };
 * @endcode
 */
class Motion {
    public:
        /**
         * @brief calculate and apply new outputs for the drivetrain
         *
         * @param drivetrain a reference to the drivetrain. This is passed here instead of the ctor to
         * simplify the creation of the motion, and to discourage the use of the drivetrain outside of
         * the calculate method
         * @param state the current position, velocity, and acceleration of the chassis
         */
        virtual void calculate(Drivetrain& drivetrain, const ChassisState& state) = 0;
        /**
         * @brief check if the motion is finished
         *
         * @return true the motion is finished
         * @return false the motion is not finished and is currently running
         */
        virtual bool isFinished() = 0;
        /**
         * @brief Destroy the Motion object
         */
        virtual ~Motion() = default;
};
} // namespace lemlib::differential