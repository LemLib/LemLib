//
// Created by Rocky Chen on 2023/5/26.
//

#pragma once
#include "lemlib/chassis/abstractChassis.hpp"

namespace lemlib {

    template<class ChassisType> class ChassisBuilder {
    public:

        ChassisBuilder();

        ~ChassisBuilder() = default;

        ChassisBuilder& withSetting(typename ChassisType::Drivetrain_t driveSetting);

        ChassisBuilder& withLateralController(AbstractChassis::ChassisController_t lateralController);

        ChassisBuilder& withAngularController(AbstractChassis::ChassisController_t angularController);

        ChassisBuilder& withStrafeController(AbstractChassis::ChassisController_t strafeController);

        ChassisBuilder& withSensor(AbstractChassis::OdomSensors_t sensors);

        ChassisType build();



    private:
        template <typename... Args>
        ChassisType buildSFINAE(std::true_type, Args... args);

        template <typename... Args>
        ChassisType buildSFINAE(std::false_type, Args... args);

        typename ChassisType::Drivetrain_t driveSetting;
        AbstractChassis::ChassisController_t lateralController;
        AbstractChassis::ChassisController_t angularController;
        AbstractChassis::ChassisController_t strafeController;
        AbstractChassis::OdomSensors_t sensors;
        bool hasDriveSetting = false;
        bool hasLateralController = false, hasAngularController = false, hasStrafeController = false;
        bool hasSensors = false;
    };



} // lemlib

#include "../src/lemlib/chassis/chassisBuilder_impl.hpp"