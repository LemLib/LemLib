
#include "lemlib/eventhandler/prosevents/buttonevents.hpp"

namespace lemlib {

PROSButtonEvent::PROSButtonEvent(std::shared_ptr<pros::Controller> controller, pros::controller_digital_e_t button, uint id) {
    this->controller = controller;
    this->button = button;
    this->id = id;
};

bool PROSButtonEvent::check() { return controller->get_digital(button); }
} // namespace lemlib