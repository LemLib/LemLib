
#include "gamepad.hpp"
#include "lemlib/eventhandler/prosevents/buttonevents.hpp"
#include "pros/misc.h"
#include <memory>

namespace lemlib {

class PROS_Gamepad : public AbstractGamepad {
    protected:
        std::shared_ptr<pros::Controller> controller;
    public:
        PROS_Gamepad(pros::controller_id_e_t controllerID, std::vector<std::string> modes,
                     std::unique_ptr<EventHandler> butHandler = nullptr,
                     std::unique_ptr<JoystickEventHandler> joyHandler = nullptr, std::string currentMode = "DEFAULT");
};

} // namespace lemlib
