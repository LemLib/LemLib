
#include "lemlib/eventhandler/prosevents/joystickevents.hpp"

namespace lemlib {

PROSJoystickEvent::PROSJoystickEvent(std::shared_ptr<pros::Controller> controller, pros::controller_analog_e_t joystick,
                                     uint id, std::function<int(int)> joystickFunction) {
    this->controller = controller;
    this->joystick = joystick;
    this->id = id;
    this->joystickFunction = joystickFunction;
   
};

bool PROSJoystickEvent::check() {
    
    return joystickFunction(joystick);
    
}

int PROSJoystickEvent::checkJoystick() {
    
    int x = joystickFunction(controller->get_analog(joystick));

    return controller->get_analog(joystick);
}

} // namespace lemlib
