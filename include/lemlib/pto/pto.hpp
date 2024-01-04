/**
 * @file include/lemlib/pto/pto.hpp
 * @author LemLib Team
 * @brief PTO class declarations.
 * @version 0.5.0
 * @date 2023-12-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "pros/motors.hpp"

#include "lemlib/pto/pneumatics.hpp"
#include "lemlib/chassis/chassis.hpp"

namespace lemlib {
/**
 * @brief PTO class
 *
 */
class PTO {
    public:
        /**
         * @brief PTO type enum (full or semi)
         * Full PTO means motors are removed from a subsystem and connected to the other
         * Semi PTO means the motors are connected to both subsystems
         */
        enum class PtoType {
            full = 0,
            semi
        };

        /**
         * @brief Construct a new PTO
         * 
         * @param pistons shared pointers to pneumatics piston group
         * @param ptoMotors shared pointer to group of motors being PTOed
         * @param drivetrain shared pointer to drivetrain (if the pto connects to drivetrain)
         * @param sysMotors shared pointer to motor group of motors in other system
         */
        PTO(PistonGroup* pistons,
            pros::MotorGroup* ptoMotors,
            Drivetrain* drivetrain,
            pros::MotorGroup* sysMotors,
            PtoType type = PtoType::full,
            const bool reversed = false);

        /**
         * @brief Construct a new PTO
         * 
         * @param pistons vector of shared pointers to pneumatic pistons
         * @param ptoMotors shared pointer to group of motors being PTOed
         * @param sysMotors1 shared pointer to motor group of motors in first system 
         * @param sysMotors2 shared pointer to motor group of motors in second system
         */
        PTO(PistonGroup* pistons,
            pros::MotorGroup* ptoMotors,
            pros::MotorGroup* sysMotors1,
            pros::MotorGroup* sysMotors2,
            PtoType type = PtoType::full,
            const bool reversed = false);

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
        PistonGroup* pistons;
        pros::MotorGroup* ptoMotors;
        std::unique_ptr<Drivetrain> drivetrain = nullptr;
        std::unique_ptr<pros::MotorGroup> sysMotors1 = nullptr;
        std::unique_ptr<pros::MotorGroup> sysMotors2 = nullptr;
        
        PtoType type;
        bool reversed;
        bool onDrivetrain;
        bool isActive = false;
};
} // namespace lemlib