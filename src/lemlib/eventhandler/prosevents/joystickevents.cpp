
#include "lemlib/eventhandler/prosevents/joystickevents.hpp"

namespace lemlib {

PROSJoystickEvent::PROSJoystickEvent(std::shared_ptr<pros::Controller> controller, pros::controller_analog_e_t joystick,
                                     uint id, std::function<int(pros::controller_analog_e_t)> joystickFunction) {
    this->controller = controller;
    this->joystick = joystick;
    this->id = id;
    this->joystickFunction = joystickFunction;

   
};

bool PROSJoystickEvent::check() {
    if (joystickFunction != nullptr) {
        return joystickFunction(joystick);
    } else {
        return controller->get_analog(joystick);
    }
}

int PROSJoystickEvent::checkJoystick() {
    return controller->get_analog(joystick);
}

} // namespace lemlib
