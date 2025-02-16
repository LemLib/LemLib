#pragma once

#include "hardware/Motor/MotorGroup.hpp"
#include "units/Pose.hpp"
#include "lemlib/ExitCondition.hpp"
#include "lemlib/PID.hpp"
#include "units/Vector2D.hpp"
#include <functional>

namespace lemlib {

struct MoveToPointParams {
        bool reversed = false;
        Number maxSpeed = 1;
        Number minSpeed = 0;
        Length earlyExitRange = 0_in;
};

struct MoveToPointSettings {
        PID angularPID;
        PID lateralPID;
        ExitConditionGroup<AngleRange> exitConditions;
        std::function<units::Pose()> poseGetter;
        lemlib::MotorGroup& leftMotors;
        lemlib::MotorGroup& rightMotors;
};

void moveToPoint(units::V2Position target, Time timeout, MoveToPointParams params, MoveToPointSettings settings);

}; // namespace lemlib