
#include "lemlib/eventhandler/joystickeventhandler.hpp"

namespace lemlib {

JoystickEventHandler::JoystickEventHandler(std::vector<std::shared_ptr<JoystickEvent>>& eventVector) {
    for (uint i = 0; i < eventVector.size(); i++) {
        joystickStates.push_back(0);
    }
}

JoystickEventHandler::~JoystickEventHandler() {
    // Destructor
}


int JoystickEventHandler::checkEvent(uint id) {
    return joystickStates[id];
}

} // namespace lemlib
