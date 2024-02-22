
#include "lemlib/eventhandler/eventhandler.hpp"
#include "lemlib/eventhandler/joystickevent.hpp"

namespace lemlib {

class JoystickEventHandler : public EventHandler {
protected:
    
    std::vector<int> joystickStates;

public: 

    JoystickEventHandler(std::vector<std::shared_ptr<JoystickEvent>>& eventVector);
    ~JoystickEventHandler();

    int checkEvent(uint id);

};

}