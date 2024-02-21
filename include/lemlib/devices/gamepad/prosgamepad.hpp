
#include "gamepad.hpp"
#include "lemlib/eventhandler/prosevents/buttonevents.hpp"
#include "pros/misc.h"
#include <memory>

namespace lemlib {

class PROS_Gamepad : public AbstractGamepad {
    protected:
        pros::Controller controller;
    public:
        PROS_Gamepad(pros::controller_id_e_t controllerID, std::vector<std::string> modes,
                     std::string currentMode = "DEFAULT",
                     std::unique_ptr<EventHandler> evHandler =
                         std::make_unique<EventHandler>(EventHandler({std::make_shared<PROSButtonEvent>(nullptr, pros::E_CONTROLLER_DIGITAL_DOWN, 0)})));
};

} // namespace lemlib
