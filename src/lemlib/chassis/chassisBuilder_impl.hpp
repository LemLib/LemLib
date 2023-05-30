//
// Created by Rocky Chen on 2023/5/26.
//
#pragma once
#include "lemlib/chassis/chassisBuilder.hpp"
#include "lemlib/chassis/differentialDrive.hpp"
#include "lemlib/chassis/holonomicDrive.hpp"
#include <iostream>
#include <type_traits>

namespace lemlib {

template <typename ChassisType> template <typename... Args>
ChassisType ChassisBuilder<ChassisType>::buildSFINAE(std::true_type, Args... args) {
    return ChassisType(args...);
}

template <typename ChassisType> template <typename... Args>
ChassisType ChassisBuilder<ChassisType>::buildSFINAE(std::false_type, Args... args) {
    throw std::runtime_error("ChassisBuilder: Invalid constructor arguments");
}

template <class ChassisType> ChassisBuilder<ChassisType>::ChassisBuilder() {
    static_assert(std::is_base_of<AbstractChassis, ChassisType>::value,
                  "\nError: ChassisBuilder<Parameter>\n"
                  "                          ^^^Parameter must be a chassis type \n");
    this->hasDriveSetting = false;
    this->hasLateralController = false;
    this->hasAngularController = false;
    this->hasStrafeController = false;
    this->hasSensors = false;
}

template <class ChassisType>
ChassisBuilder<ChassisType>& ChassisBuilder<ChassisType>::withSetting(typename ChassisType::Drivetrain_t driveSetting) {
    this->driveSetting = driveSetting;
    this->hasDriveSetting = true;
    return *this;
}

template <class ChassisType> ChassisBuilder<ChassisType>&
ChassisBuilder<ChassisType>::withLateralController(AbstractChassis::ChassisController_t lateralController) {
    this->lateralController = lateralController;
    this->hasLateralController = true;
    return *this;
}

template <class ChassisType> ChassisBuilder<ChassisType>&
ChassisBuilder<ChassisType>::withAngularController(AbstractChassis::ChassisController_t angularController) {
    this->angularController = angularController;
    this->hasAngularController = true;
    return *this;
}

template <class ChassisType> ChassisBuilder<ChassisType>&
ChassisBuilder<ChassisType>::withStrafeController(AbstractChassis::ChassisController_t strafeController) {
    static_assert(std::is_convertible<ChassisType, HolonomicDrive>::value,
                  "\nError: Only Holonomic Drives are allow to have strafeController\n");
    this->strafeController = strafeController;
    this->hasStrafeController = true;
    return *this;
}

template <class ChassisType>
ChassisBuilder<ChassisType>& ChassisBuilder<ChassisType>::withSensor(AbstractChassis::OdomSensors_t sensors) {
    this->sensors = sensors;
    this->hasSensors = true;
    return *this;
}

template <class ChassisType> ChassisType ChassisBuilder<ChassisType>::build() {
    if (this->hasDriveSetting && this->hasLateralController && this->hasAngularController &&
        this->hasStrafeController && this->hasSensors) {
        return this->buildSFINAE(
            std::is_constructible<ChassisType, typename ChassisType::Drivetrain_t, AbstractChassis::ChassisController_t,
                                  AbstractChassis::ChassisController_t, AbstractChassis::ChassisController_t,
                                  AbstractChassis::OdomSensors_t>(),
            this->driveSetting, this->lateralController, this->angularController, this->strafeController,
            this->sensors);
    }
    if (this->hasDriveSetting && this->hasLateralController && this->hasAngularController && this->hasSensors) {
        return this->buildSFINAE(
            std::is_constructible<ChassisType, typename ChassisType::Drivetrain_t, AbstractChassis::ChassisController_t,
                                  AbstractChassis::ChassisController_t, AbstractChassis::OdomSensors_t>(),
            this->driveSetting, this->lateralController, this->angularController, this->sensors);
    } else if (this->hasDriveSetting && this->hasLateralController && this->hasAngularController) {
        return this->buildSFINAE(
            std::is_constructible<ChassisType, typename ChassisType::Drivetrain_t, AbstractChassis::ChassisController_t,
                                  AbstractChassis::ChassisController_t>(),
            this->driveSetting, this->lateralController, this->angularController);
    } else {
        throw std::runtime_error("ChassisBuilder: Not all parameters supplied, failed to build");
    }
}

} // namespace lemlib
