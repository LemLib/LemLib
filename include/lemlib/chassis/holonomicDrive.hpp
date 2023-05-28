//
// Created by Rocky Chen on 2023/5/26.
//
#pragma once
#include "lemlib/chassis/holonomicDrive.hpp"
#include "pros/motors.hpp"
#include "lemlib/chassis/abstractChassis.hpp"

namespace lemlib {

    class HolonomicDrive : public AbstractChassis{
        template<class> friend class ChassisBuilder;
    public:

        typedef struct {
            pros::Motor* rf;
            pros::Motor* rb;
            pros::Motor* lf;
            pros::Motor* lb;
            float trackWidth;
            float wheelDiameter;
            float rpm;
        } Drivetrain_t;
    protected:
        HolonomicDrive() = default;
        HolonomicDrive(HolonomicDrive::Drivetrain_t driveSetting, ChassisController_t lateralController,
                       ChassisController_t angularController, ChassisController_t strafeController,
                       OdomSensors_t sensors = {nullptr, nullptr, nullptr,nullptr, nullptr});
    private:
        HolonomicDrive::Drivetrain_t driveSetting;
    };

    typedef HolonomicDrive XDrive;
    typedef HolonomicDrive MecanumDrive;

} // lemlib