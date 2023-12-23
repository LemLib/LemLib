#include "lemlib/pto/pto.hpp"

namespace lemlib {
// PTO class constructor (drivetrain to subsystem)
PTO::PTO(const std::shared_ptr<PistonGroup>&      pistons,
         const std::shared_ptr<pros::MotorGroup>& ptoMotors,
         const std::shared_ptr<Drivetrain>&       drivetrain,
         const std::shared_ptr<pros::MotorGroup>& sysMotors)
    : pistons(pistons),
      ptoMotors(ptoMotors),
      drivetrain(drivetrain),
      sysMotors1(sysMotors) {
    // Set using drivetrain flag to true
    onDrivetrain = true;
}

// PTO class constructor (two separate subsystems)
PTO::PTO(const std::shared_ptr<PistonGroup>&      pistons,
         const std::shared_ptr<pros::MotorGroup>& ptoMotors,
         const std::shared_ptr<pros::MotorGroup>& sysMotors1,
         const std::shared_ptr<pros::MotorGroup>& sysMotors2)
    : pistons(pistons),
      ptoMotors(ptoMotors),
      sysMotors1(sysMotors1),
      sysMotors2(sysMotors2) {
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
