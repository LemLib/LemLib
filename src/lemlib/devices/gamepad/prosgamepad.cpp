
#include "lemlib/devices/gamepad/prosgamepad.hpp"
#include "lemlib/eventhandler/eventhandler.hpp"
#include "lemlib/eventhandler/prosevents/buttonevents.hpp"
#include "lemlib/eventhandler/prosevents/joystickevents.hpp"
#include "pros/misc.h"
#include "pros/misc.hpp"
#include <memory>

namespace lemlib {

PROS_Gamepad::PROS_Gamepad(pros::controller_id_e_t controllerID, std::vector<std::string> modes,
                           std::unique_ptr<EventHandler> butHandler, std::unique_ptr<JoystickEventHandler> joyHandler,
                           std::string currentMode) {
    this->controller = std::make_shared<pros::Controller>(controllerID);

    if (butHandler != nullptr) {
        this->butHandler = std::move(butHandler);
    } else {
        lemlib::PROSButtonEvent XEvent(this->controller, pros::E_CONTROLLER_DIGITAL_X, pros::E_CONTROLLER_DIGITAL_X);
        lemlib::PROSButtonEvent BEvent(this->controller, pros::E_CONTROLLER_DIGITAL_B, pros::E_CONTROLLER_DIGITAL_B);
        lemlib::PROSButtonEvent YEvent(this->controller, pros::E_CONTROLLER_DIGITAL_Y, pros::E_CONTROLLER_DIGITAL_Y);
        lemlib::PROSButtonEvent AEvent(this->controller, pros::E_CONTROLLER_DIGITAL_A, pros::E_CONTROLLER_DIGITAL_A);
        lemlib::PROSButtonEvent L1Event(this->controller, pros::E_CONTROLLER_DIGITAL_L1, pros::E_CONTROLLER_DIGITAL_L1);
        lemlib::PROSButtonEvent L2Event(this->controller, pros::E_CONTROLLER_DIGITAL_L2, pros::E_CONTROLLER_DIGITAL_L2);
        lemlib::PROSButtonEvent R1Event(this->controller, pros::E_CONTROLLER_DIGITAL_R1, pros::E_CONTROLLER_DIGITAL_R1);
        lemlib::PROSButtonEvent R2Event(this->controller, pros::E_CONTROLLER_DIGITAL_R2, pros::E_CONTROLLER_DIGITAL_R2);
        lemlib::PROSButtonEvent UpEvent(this->controller, pros::E_CONTROLLER_DIGITAL_UP, pros::E_CONTROLLER_DIGITAL_UP);
        lemlib::PROSButtonEvent DownEvent(this->controller, pros::E_CONTROLLER_DIGITAL_DOWN,
                                          pros::E_CONTROLLER_DIGITAL_DOWN);
        lemlib::PROSButtonEvent LeftEvent(this->controller, pros::E_CONTROLLER_DIGITAL_LEFT,
                                          pros::E_CONTROLLER_DIGITAL_LEFT);
        lemlib::PROSButtonEvent RightEvent(this->controller, pros::E_CONTROLLER_DIGITAL_RIGHT,
                                           pros::E_CONTROLLER_DIGITAL_RIGHT);

        std::vector<std::shared_ptr<lemlib::Event>> buttonsEvents(
            {std::make_shared<lemlib::PROSButtonEvent>(XEvent), std::make_shared<lemlib::PROSButtonEvent>(AEvent),
             std::make_shared<lemlib::PROSButtonEvent>(BEvent), std::make_shared<lemlib::PROSButtonEvent>(YEvent),
             std::make_shared<lemlib::PROSButtonEvent>(L1Event), std::make_shared<lemlib::PROSButtonEvent>(L2Event),
             std::make_shared<lemlib::PROSButtonEvent>(R1Event), std::make_shared<lemlib::PROSButtonEvent>(R2Event),
             std::make_shared<lemlib::PROSButtonEvent>(UpEvent), std::make_shared<lemlib::PROSButtonEvent>(DownEvent),
             std::make_shared<lemlib::PROSButtonEvent>(LeftEvent),
             std::make_shared<lemlib::PROSButtonEvent>(RightEvent)});

        this->butHandler = std::make_unique<lemlib::EventHandler>(buttonsEvents);
    }

    if (joyHandler != nullptr) {
        this->joyHandler = std::move(joyHandler);
    } else {
        lemlib::PROSJoystickEvent leftYJoyEvent(this->controller, pros::E_CONTROLLER_ANALOG_LEFT_Y,
                                                pros::E_CONTROLLER_ANALOG_LEFT_Y);
        lemlib::PROSJoystickEvent rightYJoyEvent(this->controller, pros::E_CONTROLLER_ANALOG_RIGHT_Y,
                                                 pros::E_CONTROLLER_ANALOG_RIGHT_Y);
        lemlib::PROSJoystickEvent leftXJoyEvent(this->controller, pros::E_CONTROLLER_ANALOG_LEFT_X,
                                                pros::E_CONTROLLER_ANALOG_LEFT_X);
        lemlib::PROSJoystickEvent rightXJoyEvent(this->controller, pros::E_CONTROLLER_ANALOG_RIGHT_X,
                                                 pros::E_CONTROLLER_ANALOG_RIGHT_X);

        std::vector<std::shared_ptr<lemlib::JoystickEvent>> joystickEvents(
            {std::make_shared<lemlib::PROSJoystickEvent>(leftYJoyEvent),
             std::make_shared<lemlib::PROSJoystickEvent>(rightYJoyEvent),
             std::make_shared<lemlib::PROSJoystickEvent>(leftXJoyEvent),
             std::make_shared<lemlib::PROSJoystickEvent>(rightXJoyEvent)});

        this->joyHandler = std::make_unique<lemlib::JoystickEventHandler>(joystickEvents);
    }
}

} // namespace lemlib
