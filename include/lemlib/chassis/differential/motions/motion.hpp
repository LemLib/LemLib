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
 *     std::unique_ptr<Drivetrain> calculate(std::unique_ptr<Drivetrain> drivetrain,
 *                                           const DrivetrainState& state) override {
 *       drivetrain->left.move(127);
 *       drivetrain->right.move(127);
 *       m_finished = false; // this motion never stops
 *       return std::move(drivetrain); // return ownership of the drivetrain
 *     }
 * };
 * @endcode
 */
class Motion {
    protected:
        bool m_finished = false; /** whether the motion is finished or not */
    public:
        /**
         * @brief calculate and apply new outputs for the drivetrain
         *
         * @param drivetrain a unique pointer to the drivetrain. This should be returned
         * @param state the current position, velocity, and acceleration of the chassis
         * @return std::unique_ptr<Drivetrain> the unique pointer that was passed as an argument
         *
         */
        virtual std::unique_ptr<Drivetrain> calculate(std::unique_ptr<Drivetrain> drivetrain,
                                                      const DrivetrainState& state) = 0;
        /**
         * @brief check if the motion is finished
         *
         * This function simply returns the value of m_finished, which is set by the child class
         * Once m_finished is set to true, the motion is considered finished and the calculate method
         * should not be called again.
         *
         * @return true the motion is finished or does not exist
         * @return false the motion is not finished and is currently running
         *
         * @b Example
         * @code {.cpp}
         * if (motion->isFinished()) {
         *   // do something
         * }
         * @endcode
         */
        virtual bool isFinished() = 0;
        /**
         * @brief Destroy the Motion object
         *
         */
        virtual ~Motion();
};

} // namespace lemlib::differential