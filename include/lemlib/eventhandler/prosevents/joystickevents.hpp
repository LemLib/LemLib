#pragma once

#include "lemlib/eventhandler/joystickevent.hpp"
#include "pros/misc.h"
#include "pros/rtos.h"
#include "pros/device.hpp"

namespace lemlib {

class PROSJoystickEvent : public JoystickEvent {
    protected:
        std::shared_ptr<pros::Controller> controller;
        pros::controller_analog_e_t joystick;
        std::function<int(int)> joystickFunction;
    public:
        PROSJoystickEvent(std::shared_ptr<pros::Controller> controller, pros::controller_analog_e_t joystick, uint id,
                          std::function<int(int)> joystickFunction = nullptr);

        bool check() override;

        int checkJoystick() override;
};

} // namespace lemlib
