
#include "lemlib/devices/gamepad/prosgamepad.hpp"
#include "pros/misc.h"

namespace lemlib {

PROS_Gamepad::PROS_Gamepad(pros::controller_id_e_t controllerID, std::vector<std::string> modes,
                           std::string currentMode,
                           std::unique_ptr<EventHandler> evHandler)
    : controller(controllerID) {
    this->modes = modes;
    this->currentMode = currentMode;
    this->evHandler = std::move(evHandler);
    
}

} // namespace lemlib
