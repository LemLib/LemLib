
#include "lemlib/eventhandler/prosevents/buttonevents.hpp"

namespace lemlib {

PROSButtonEvent::PROSButtonEvent(std::shared_ptr<pros::Controller> controller, pros::controller_digital_e_t button,
                                 uint id, std::function<bool()> functionTrue, std::function<bool()> functionFalse) {
    this->controller = controller;
    this->button = button;
    this->id = id;
    this->functionTrue = functionTrue;
    this->functionFalse = functionFalse;
};

bool PROSButtonEvent::check() {
    bool buttonState = controller->get_digital(button);

    if (buttonState && functionTrue != nullptr) {
        functionTrue();
    } else if (!buttonState && functionFalse != nullptr) {
        functionFalse();
    }

    return buttonState;
}
} // namespace lemlib