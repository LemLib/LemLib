#pragma once

#include "lemlib/eventhandler/event.hpp"
#include "pros/misc.h"
#include "pros/rtos.h"
#include "pros/device.hpp"

namespace lemlib {

class JoystickEvent : public Event {
    protected:
        uint id;
    public:
        
        virtual int checkJoystick() = 0;
};

} // namespace lemlib