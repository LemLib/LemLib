
#include "lemlib/devices/gamepad/prosgamepad.hpp"
#include "pros/misc.h"

namespace lemlib {

PROS_Gamepad::PROS_Gamepad(pros::controller_id_e_t controllerID, std::vector<std::string> modes,
                           std::string currentMode,
                           std::vector<std::function<bool(pros::controller_digital_e_t)>> buttonFunctions,
                           std::vector<std::function<int(pros::controller_analog_e_t)>> joystickFunctions)
    : controller(controllerID) {
    this->modes = modes;
    this->currentMode = currentMode;
    this->buttonFunctions = buttonFunctions;
    this->joystickFunctions = joystickFunctions;
    
}

void PROS_Gamepad::autoButtonFunctions() {
    // Starting from L1, going to the end of the buttonFunctions vector
    for (int i = pros::E_CONTROLLER_DIGITAL_L1; i < pros::E_CONTROLLER_DIGITAL_A; i++) {
        if (evHandler->isValidID(i)) {
            if (evHandler->checkEvent(i)) { buttonFunctions[i]((pros::controller_digital_e_t)i); }
        }
    }
}

} // namespace lemlib
