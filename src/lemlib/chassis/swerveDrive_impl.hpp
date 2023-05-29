//
// Created by Rocky Chen on 2023/5/28.
//
#pragma once
#include "lemlib/chassis/swerveDrive.hpp"

namespace lemlib {
template <SwerveType_t SwerveType>
SwerveDrive<SwerveType>::SwerveDrive(SwerveDrive<SwerveType>::Drivetrain_t driveSetting,
                                     AbstractChassis::ChassisController_t lateralSettings,
                                     AbstractChassis::ChassisController_t angularSettings,
                                     AbstractChassis::ChassisController_t strafeSettings,
                                     AbstractChassis::OdomSensors_t odomSensors)
    : HolonomicDrive(driveSetting, lateralSettings, angularSettings, strafeSettings, odomSensors){}
} // lemlib