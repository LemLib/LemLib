//
// Created by Rocky Chen on 2023/5/28.
//

#pragma once
#include "lemlib/chassis/abstractChassis.hpp"
#include "lemlib/chassis/holonomicDrive.hpp"
#include "pros/motors.hpp"

namespace lemlib {

enum class SwerveType_t{
    Coaxial, Differential, Single, Bruh
};


template<SwerveType_t SwerveType> class SwerveDrive : public HolonomicDrive{
    public:
        SwerveDrive() = delete;
    protected:
        SwerveDrive(SwerveDrive<SwerveType>::Drivetrain_t driveSetting, ChassisController_t lateralSettings, ChassisController_t angularSettings,
                    ChassisController_t strafeSettings, OdomSensors_t odomSensors);

//        void calibrate() override;
    private:
};

template<> class SwerveDrive<SwerveType_t::Coaxial> : public HolonomicDrive{ // template specialization
    public:
        typedef struct {
                pros::Motor* rf_move;
                pros::Motor* rf_dir_control;
                pros::Motor* rb_move;
                pros::Motor* rb_dir_control;
                pros::Motor* lf_move;
                pros::Motor* lf_dir_control;
                pros::Motor* lb_move;
                pros::Motor* lb_dir_control;
                float trackWidth;
                float wheelDiameter;
                float rpm;
        } Drivetrain_t;
};


template<> class SwerveDrive<SwerveType_t::Differential> : public HolonomicDrive{ // template specialization
    public:
        using MotorPairs = std::pair<pros::Motor_Group*, pros::Motor_Group*>;
        typedef struct {
                MotorPairs rf;
                MotorPairs rb;
                MotorPairs lf;
                MotorPairs lb;
                float trackWidth;
                float wheelDiameter;
                float rpm;
        } Drivetrain_t;
};

template<> class SwerveDrive<SwerveType_t::Single> : public HolonomicDrive{ // template specialization
    public:
        typedef struct {
                pros::Motor* rf;
                pros::Motor* rb;
                pros::Motor* lf;
                pros::Motor* lb;
                pros::Motor* directionalControl;
                float trackWidth;
                float wheelDiameter;
                float rpm;
        } Drivetrain_t;
};

template<> class SwerveDrive<SwerveType_t::Bruh> : public HolonomicDrive{ // template specialization
    public:
        typedef struct {
        } Drivetrain_t;
};



} // lemlib

#include "../src/lemlib/chassis/swerveDrive_impl.hpp"