
#include "lemlib/eventhandler/event.hpp"
#include "pros/misc.h"
#include "pros/rtos.h"
#include "pros/device.hpp"

namespace lemlib {

class PROSJoystickEvent : public Event {
    protected:
        std::shared_ptr<pros::Controller> controller;
        pros::controller_analog_e_t joystick;
        std::function<int(pros::controller_analog_e_t)> joystickFunction;
    public:
        PROSJoystickEvent(std::shared_ptr<pros::Controller> controller, pros::controller_analog_e_t joystick, uint id,
                          std::function<int(pros::controller_analog_e_t)> joystickFunction = nullptr);
                          
        bool check() override;

        int getJoystickValue();
};

} // namespace lemlib
