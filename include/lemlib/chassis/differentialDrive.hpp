//
// Created by Rocky Chen on 2023/5/26.
//
#pragma once
#include "lemlib/chassis/abstractChassis.hpp"

namespace lemlib {

    class DifferentialDrive : public AbstractChassis{
    public:

        typedef struct {
            pros::Motor_Group* leftMotors;
            pros::Motor_Group* rightMotors;
            float trackWidth;
            float wheelDiameter;
            float rpm;
        } Drivetrain_t;
    protected:

        DifferentialDrive() = default;
        DifferentialDrive(DifferentialDrive::Drivetrain_t driveSetting, ChassisController_t lateralController,
                          ChassisController_t angularController,
                          OdomSensors_t sensors = {nullptr, nullptr, nullptr,nullptr, nullptr});

    public:
        ~DifferentialDrive() = default;


    private:
        DifferentialDrive::Drivetrain_t drivetrain;
    };

} // lemlib

