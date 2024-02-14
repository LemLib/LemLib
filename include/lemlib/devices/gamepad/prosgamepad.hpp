
#include "gamepad.hpp"
#include "pros/misc.h"

namespace lemlib {

class PROS_Gamepad : public AbstractGamepad {
    protected:
        std::vector<std::function<bool(pros::controller_digital_e_t)>> buttonFunctions;
        std::vector<std::function<int(pros::controller_analog_e_t)>> joystickFunctions;

        pros::Controller controller;

        void autoButtonFunctions() override;

    public:
        
        PROS_Gamepad(pros::controller_id_e_t controllerID, std::vector<std::string> modes,
                     std::string currentMode = "DEFAULT",
                     std::vector<std::function<bool(pros::controller_digital_e_t)>> buttonFunctions = {},
                     std::vector<std::function<int(pros::controller_analog_e_t)>> joystickFunctions = {});

        
};

} // namespace lemlib
