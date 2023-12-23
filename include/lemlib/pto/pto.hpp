/**
 * @file include/lemlib/pto.hpp
 * @author LemLib Team
 * @brief PTO class declarations
 * @version 0.4.7
 * @date 2023-12-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <memory>

#include "pros/adi.hpp"
#include "pros/motor_group.hpp"

#include "lemlib/chassis/differential.hpp"
#include "lemlib/pto/pneumatics.hpp"

namespace lemlib {
class PTO {
    public:
        /**
         * @brief Construct a new PTO
         * 
         * @param pistons shared pointers to pneumatics piston group
         * @param ptoMotors shared pointer to group of motors being PTOed
         * @param drivetrain shared pointer to drivetrain (if the pto connects to drivetrain)
         * @param sysMotors shared pointer to motor group of motors in other system
         */
        PTO(const PistonGroup& pistons,
            const pros::MotorGroup& ptoMotors,
            const Drivetrain& drivetrain,
            const pros::MotorGroup& sysMotors);
        
        /**
         * @brief Construct a new PTO
         * 
         * @param pistons vector of shared pointers to pneumatic pistons
         * @param ptoMotors shared pointer to group of motors being PTOed
         * @param sysMotors1 shared pointer to motor group of motors in first system 
         * @param sysMotors2 shared pointer to motor group of motors in second system
         */
        PTO(const PistonGroup& pistons,
            const pros::MotorGroup& ptoMotors,
            const pros::MotorGroup& sysMotors1,
            const pros::MotorGroup& sysMotors2);

        /**
         * @brief returns the PTO state
         *
         * @return Whether or not the PTO is active
         */
        bool getState();

        /**
         * TODO: implement logic
         * @brief returns the PTO state
         *
         * @param targetState the state for the PTO to move to
         */
        void moveToState(bool targetState);

        /**
         * @brief toggles the PTO state
         */
        void toggle();
    private:
        std::unique_ptr<PistonGroup> pistons;
        std::shared_ptr<pros::MotorGroup> motors;
        std::shared_ptr<Drivetrain> drivetrain = nullptr;
        std::shared_ptr<pros::MotorGroup> sysMotors1 = nullptr;
        std::shared_ptr<pros::MotorGroup> sysMotors2 = nullptr;
        bool state = false;
        bool onDrivetrain;

};
} // namespace lemlib