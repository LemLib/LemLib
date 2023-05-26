//
// Created by Rocky Chen on 2023/5/26.
//
#pragma once
#include <iostream>
#include "lemlib/chassis/chassisBuilder.hpp"
#include "lemlib/chassis/holonomicDrive.hpp"

namespace lemlib {

    template<class ChassisType>
    ChassisBuilder<ChassisType>::ChassisBuilder() {
        static_assert(std::is_base_of<AbstractChassis, ChassisType>::value,
                      "\nError: ChassisBuilder<Parameter>\n"
                      "                          ^^^Parameter must be a chassis type \n");
        this->hasDriveSetting = false;
        this->hasLateralController = false;
        this->hasAngularController = false;
        this->hasStrafeController = false;
        this->hasSensors = false;
    }

    template<class ChassisType>
    ChassisBuilder<ChassisType>& ChassisBuilder<ChassisType>::withSetting(
            typename ChassisType::Drivetrain_t driveSetting) {
        this->driveSetting = driveSetting;
        this->hasDriveSetting = true;
        return *this;
    }

    template<class ChassisType>
    ChassisBuilder<ChassisType>& ChassisBuilder<ChassisType>::withLateralController(
            AbstractChassis::ChassisController_t lateralController) {
        this->lateralController = lateralController;
        this->hasLateralController = true;
        return *this;
    }

    template<class ChassisType>
    ChassisBuilder<ChassisType>& ChassisBuilder<ChassisType>::withAngularController(
            AbstractChassis::ChassisController_t angularController) {
        this->angularController = angularController;
        this->hasAngularController = true;
        return *this;
    }


    template<class ChassisType>
    ChassisBuilder<ChassisType>& ChassisBuilder<ChassisType>::withStrafeController(
            AbstractChassis::ChassisController_t strafeController) {
        static_assert(std::is_convertible<HolonomicDrive, ChassisType>::value,
                "\nError: Only Holonomic Drives are allow to have strafeController\n");
        this->strafeController = strafeController;
        this->hasStrafeController = true;
        return *this;
    }

    template<class ChassisType>
    ChassisBuilder<ChassisType>& ChassisBuilder<ChassisType>::withSensor(AbstractChassis::OdomSensors_t sensors) {
        this->sensors = sensors;
        this->hasSensors = true;
        return *this;
    }


    template<class ChassisType>
    std::shared_ptr<ChassisType> ChassisBuilder<ChassisType>::build(){
        if(this->hasDriveSetting && this->hasLateralController && this->hasAngularController
        && this->hasStrafeController && this->hasSensors && std::is_convertible<HolonomicDrive, ChassisType>::value){
            std::cout<< "ChassisBuilder: Build successful"<< std::endl;
            return ChassisType(this->driveSetting,
                               this->hasLateralController,
                               this->hasAngularController,
                               this->hasStrafeController,
                               this->sensors);
        }
        else if(this->hasDriveSetting && this->hasLateralController && this->hasAngularController && this->hasSensors){
            std::cout<< "ChassisBuilder: Build successful"<< std::endl;
            return ChassisType(this->driveSetting,
                               this->hasLateralController,
                               this->hasAngularController,
                               this->sensors);
        }
        else if(this->hasDriveSetting && this->hasLateralController && this->hasAngularController){
            std::cout<< "ChassisBuilder: Build successful"<< std::endl;
            return ChassisType(this->driveSetting,
                               this->hasLateralController,
                               this->hasAngularController);
        }
        else{
            throw std::runtime_error("ChassisBuilder: Not all parameters supplied, failed to build");
        }
    }

} // lemlib