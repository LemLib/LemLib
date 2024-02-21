
#include "lemlib/eventhandler/prosevents/buttonevents.hpp"

namespace lemlib {

PROSButtonEvent::PROSButtonEvent(std::shared_ptr<pros::Controller> controller, pros::controller_digital_e_t button,
                                 uint id, std::function<bool()> functionTrue, std::function<bool()> functionFalse) {
    this->controller = controller;
    this->button = button;
    this->id = id;
    this->functionTrue = functionTrue;
    this->functionFalse = functionFalse;

    try {
        if (this->id > -1) {
            std::cout << "Proper identification.";
        } else {
            std::cout << "Negative ID Value not allowed. Throwing exception. ";
            throw std::exception();
        }
    } catch (std::exception& e) { std::cout << "Improper identification. Add ID Parameter to the Event."; }
};

bool PROSButtonEvent::check() { 

    bool buttonState = controller->get_digital(button);

    if (buttonState) {
        functionTrue();
    }
    else {
        functionFalse();
    }
    
    return buttonState; 
    
}
} // namespace lemlib