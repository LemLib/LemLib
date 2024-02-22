
#include "lemlib/eventhandler/joystickeventhandler.hpp"
#include "lemlib/eventhandler/joystickevent.hpp"

namespace lemlib {

JoystickEventHandler::JoystickEventHandler(std::vector<std::shared_ptr<JoystickEvent>>& joyVector) {
    joystickStates.resize(joyVector.size());
    this->joyVector = joyVector;

    // Bubble Sort the Event IDs

    for (int i = 0; i < this->joyVector.size(); i++) {
        for (int j = 0; j < this->joyVector.size() - i - 1; j++) {
            if (this->joyVector.at(j)->getId() > this->joyVector.at(j + 1)->getId()) {
                std::swap(this->joyVector.at(j), this->joyVector.at(j + 1));
            }
        }
    }

    std::cout << "    Event Vector:  ";
    for (auto& event : this->joyVector) { std::cout << " " << event->getId() << ", "; }
    std::cout << std::endl;
    for (int i = 0; i < this->joyVector.size() - 1; i++) {
        // If the ID is the same as the next ID, then throw an exception.
        if (this->joyVector.at(i)->getId() != this->joyVector.at(i + 1)->getId()) {
            std::cout << "Valid IDs: " << this->joyVector.at(i)->getId() << " != " << this->joyVector.at(i + 1)->getId()
                      << std::endl;
        } else {
            std::cout << "Event IDs at " << i << ", " << i + 1 << " of " << this->joyVector.at(i)->getId() << " = "
                      << this->joyVector.at(i + 1)->getId() << " are not unique. ";
            std::cout << "Throwing exception." << std::endl;
            throw std::exception();
            pros::delay(200);
        }
    }
}

JoystickEventHandler::~JoystickEventHandler() {
    // Destructor
}

int JoystickEventHandler::checkEvent(uint id) {

    int eventResult = 0;
    int index = 0;

    for (auto event : eventVector) {
        if (id == event->getId()) {
            eventResult = event->check();
        }
        
    }

    // { 0, 3, 5, 6 }
    // ID = 3, index = 1

    for (int i = 0; i < joyVector.size(); i++) {
        if (id <= joyVector.at(i)->getId()) {
            index = i;
        }
    }

    joystickStates[id] = eventResult;
    
    return joystickStates[id];
}

void JoystickEventHandler::startAsyncTask() {
    while (true) {
        for (auto event : joyVector) { eventStates[event->getId()] = checkEvent(event->getId()); }
        pros::delay(20);
    }
}

} // namespace lemlib
