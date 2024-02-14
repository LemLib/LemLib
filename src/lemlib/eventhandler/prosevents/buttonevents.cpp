
#include "lemlib/eventhandler/prosevents/buttonevents.hpp"

namespace lemlib {

PROSButtonEvent::PROSButtonEvent(std::shared_ptr<pros::Controller> controller, pros::controller_digital_e_t button,
                                 uint id) {
    this->controller = controller;
    this->button = button;
    this->id = id;

    try {
        if (this->id > -1) {
            std::cout << "Proper identification.";
        } else {
            std::cout << "Negative ID Value not allowed. Throwing exception. ";
            throw std::exception();
        }
    } catch (std::exception& e) { std::cout << "Improper identification. Add ID Parameter to the Event."; }
};

bool PROSButtonEvent::check() { return controller->get_digital(button); }
} // namespace lemlib