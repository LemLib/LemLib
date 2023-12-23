#include "lemlib/pto/pto.hpp"

namespace lemlib {
// PTO class constructor (drivetrain to subsystem)
PTO::PTO(const PistonGroup& pistons,
         const pros::MotorGroup& ptoMotors,
         const Drivetrain& drivetrain,
         const pros::MotorGroup& sysMotors)
    : pistons(std::make_unique<PistonGroup>(pistons)),
      motors(std::make_shared<pros::MotorGroup>(ptoMotors)),
      drivetrain(std::make_shared<Drivetrain>(drivetrain)),
      sysMotors1(std::make_shared<pros::MotorGroup>(sysMotors)) {
    // Set using drivetrain flag to true
    onDrivetrain = true;
}

// PTO class constructor (two separate subsystems)
PTO::PTO(const PistonGroup& pistons,
         const pros::MotorGroup& ptoMotors,
         const pros::MotorGroup& sysMotors1,
         const pros::MotorGroup& sysMotors2)
    : pistons(std::make_unique<PistonGroup>(pistons)),
      motors(std::make_shared<pros::MotorGroup>(ptoMotors)),
      sysMotors1(std::make_shared<pros::MotorGroup>(sysMotors1)),
      sysMotors2(std::make_shared<pros::MotorGroup>(sysMotors2)) {
    // Set using drivetrain flag to false
    onDrivetrain = false;
}

// PTO state getter
bool PTO::getState() {return state;}

// PTO move to state
void PTO::moveToState(bool targetState) {
    // TODO: implement logic
    if (targetState) {

    }
}

// PTO toggle
void PTO::toggle() {
    this->moveToState(!state);
}

} // namespace lemlib
