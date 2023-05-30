//
// Created by Rocky Chen on 2023/5/26.
//

#include "lemlib/chassis/holonomicDrive.hpp"
#include "lemlib/chassis/abstractChassis.hpp"

namespace lemlib {

HolonomicDrive::HolonomicDrive(HolonomicDrive::Drivetrain_t driveSetting, ChassisController_t lateralController,
                               ChassisController_t angularController, ChassisController_t strafeController,
                               OdomSensors_t sensors)
    : AbstractChassis(lateralController, angularController, strafeController, sensors), driveSetting(driveSetting) {};

} // namespace lemlib
