#pragma once

// this file is used to configure default values used by motion algorithms used in LemLib

#include "ExitCondition.hpp"
#include "PID.hpp"
#include "hardware/Motor/MotorGroup.hpp"
#include "units/Pose.hpp"
#include <functional>

extern const lemlib::PID angular_pid;
extern const lemlib::PID lateral_pid;

extern const std::function<units::Pose()> pose_getter;

extern lemlib::MotorGroup left_motors;
extern lemlib::MotorGroup right_motors;

extern const lemlib::ExitConditionGroup<AngleRange> angular_exit_conditions;
extern const lemlib::ExitConditionGroup<Length> lateral_exit_conditions;

extern const Length track_width;

extern const Number drift_compensation;

extern const Number angular_slew;
extern const Number lateral_slew;