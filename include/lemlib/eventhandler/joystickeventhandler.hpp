#pragma once

#include "lemlib/eventhandler/eventhandler.hpp"
#include "lemlib/eventhandler/joystickevent.hpp"

namespace lemlib {

class JoystickEventHandler : public EventHandler {
protected:
    
    std::vector<int> joystickStates;
    std::vector<std::shared_ptr<JoystickEvent>> joyVector;

public: 

    JoystickEventHandler(std::vector<std::shared_ptr<JoystickEvent>>& joyVector);
    ~JoystickEventHandler();

    int checkEvent(uint id);

    void startAsyncTask();

};

}