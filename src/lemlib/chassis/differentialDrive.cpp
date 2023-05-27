//
// Created by Rocky Chen on 2023/5/26.
//

#include "lemlib/chassis/differentialDrive.hpp"
#include "lemlib/chassis/abstractChassis.hpp"

namespace lemlib {

    DifferentialDrive::DifferentialDrive(DifferentialDrive::Drivetrain_t driveSetting,
                                         lemlib::AbstractChassis::ChassisController_t lateralController,
                                         lemlib::AbstractChassis::ChassisController_t angularController,
                                         lemlib::AbstractChassis::OdomSensors_t sensors) :
                                         AbstractChassis(lateralController,
                                                         angularController,
                                                         sensors),
                                         drivetrain(driveSetting){};


} // lemlib