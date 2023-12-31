/**
 * @file src/lemlib/pto/pto.cpp
 * @author LemLib Team
 * @brief definitions for PTO.
 * @version 0.5.0
 * @date 2023-12-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "lemlib/pto/pto.hpp"

namespace lemlib {

// PTO class constructor (drivetrain to subsystem)
PTO::PTO(PistonGroup*      pistons,
         pros::MotorGroup* ptoMotors,
         Drivetrain*       drivetrain,
         pros::MotorGroup* sysMotors,
         PtoType           type,
         const bool        reversed)
    : pistons(pistons),
      ptoMotors(ptoMotors),
      drivetrain(drivetrain),
      sysMotors1(sysMotors),
      type(type),
      reversed(reversed) {
    // Set using drivetrain flag to true
    onDrivetrain = true;
}

// PTO class constructor (two separate subsystems)
PTO::PTO(PistonGroup*      pistons,
         pros::MotorGroup* ptoMotors,
         pros::MotorGroup* sysMotors1,
         pros::MotorGroup* sysMotors2,
         PtoType           type,
         const bool        reversed)
    : pistons(pistons),
      ptoMotors(ptoMotors),
      sysMotors1(sysMotors1),
      sysMotors2(sysMotors2) {
    // Set using drivetrain flag to false
    onDrivetrain = false;
}

// PTO state getter
bool PTO::getState() {return isActive;}

// PTO move to state
void PTO::moveToState(bool targetState) {
    // TODO: implement logic
    if (onDrivetrain && targetState) {
        
    }
}

// PTO toggle
void PTO::toggle() {
    this->moveToState(!isActive);
}
} // namespace lemlib