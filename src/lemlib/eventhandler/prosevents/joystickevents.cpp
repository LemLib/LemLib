
#include "lemlib/eventhandler/prosevents/joystickevents.hpp"

namespace lemlib {

PROSJoystickEvent::PROSJoystickEvent(std::shared_ptr<pros::Controller> controller, pros::controller_analog_e_t joystick,
                                     uint id, std::function<int(pros::controller_analog_e_t)> joystickFunction) {
    this->controller = controller;
    this->joystick = joystick;
    this->id = id;
    this->joystickFunction = joystickFunction;

    try {
        if (this->id > -1) {
            std::cout << "Proper identification.";
        } else {
            std::cout << "Negative ID Value not allowed. Throwing exception. ";
            throw std::exception();
        }
    } catch (std::exception& e) { std::cout << "Improper identification. Add ID Parameter to the Event."; }
};

bool PROSJoystickEvent::check() {
    if (joystickFunction != nullptr) {
        return joystickFunction(joystick);
    } else {
        return controller->get_analog(joystick);
    }
}

int PROSJoystickEvent::getJoystickValue() {
    return controller->get_analog(joystick);
}

} // namespace lemlib
