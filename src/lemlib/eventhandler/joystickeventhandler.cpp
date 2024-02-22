
#include "lemlib/eventhandler/joystickeventhandler.hpp"
#include "lemlib/eventhandler/joystickevent.hpp"

namespace lemlib {

JoystickEventHandler::JoystickEventHandler(std::vector<std::shared_ptr<JoystickEvent>>& joyVector) {
    for (uint i = 0; i < eventVector.size(); i++) {
        joystickStates.push_back(0);
    }
    this->joyVector = joyVector;
}

JoystickEventHandler::~JoystickEventHandler() {
    // Destructor
}


int JoystickEventHandler::checkEvent(uint id) {
    joystickStates[id] = joyVector[id]->checkJoystick();
    return joystickStates[id];
}

void JoystickEventHandler::startAsyncTask() {
    while (true) {
        for (auto event : joyVector) {
            eventStates[event->getId()] = checkEvent(event->getId());
        }
        pros::delay(20);
    }
}

} // namespace lemlib
