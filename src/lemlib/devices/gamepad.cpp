#include "lemlib/devices/gamepad.hpp"
#include "pros/misc.h"
#include <memory>
#include <utility>
#include <iostream>
#include <vector>

namespace lemlib {

ButtonMapping::ButtonMapping(pros::controller_digital_e_t button, std::string mode,
                             std::pair<std::function<int(int)>, std::function<int(int)>> function) {
    this->button = button;
    functions.push_back({mode, function});
}

// Acts like a tag.
pros::controller_digital_e_t ButtonMapping::getButton() { return button; }

void ButtonMapping::addModeAndFunction(std::string mode,
                                       std::pair<std::function<int(int)>, std::function<int(int)>> function) {
    functions.push_back({mode, function});
}

void ButtonMapping::runFunction(std::string mode, bool buttonState, int func) {
    // Loops through all the functions and runs the one that matches the mode.

    for (std::pair<std::string, std::pair<std::function<int(int)>, std::function<int(int)>>>& i : functions) {
        if (i.first == mode) { // If the mode matches,

            if (buttonState == true) {
                if (i.second.second != nullptr) { // If the function for when its true is not a null pointer
                    i.second.second(func); // Run the function pointer for the true state
                }
            } else {
                if (i.second.first != nullptr) { // If the function for when its false is not a null pointer
                    i.second.first(func); // Run the function pointer for the false state
                }
            }
        }
    }
}

JoystickMapping::JoystickMapping(pros::controller_analog_e_t joystick, std::string mode,
                                 std::function<int(int)> function) {
    this->joystick = joystick;
    functions.push_back({mode, function});
}

// Acts like a tag
pros::controller_analog_e_t JoystickMapping::getJoystick() { return joystick; }

void JoystickMapping::addModeAndFunction(std::string mode, std::function<int(int)> function) {
    functions.push_back({mode, function});
}

void JoystickMapping::runFunction(std::string mode, int joystickValue) {
    // Loops through all the functions and runs the one that matches the mode.
    for (std::pair<std::string, std::function<int(int)>> i : functions) {
        if (i.first == mode) { // If the mode matches,
            if (i.second != nullptr) { // If the function is not a null pointer
                i.second(joystickValue); // Run the function pointer
            }
        }
    }
}

ControllerValues::ControllerValues() {}

uint8_t ControllerValues::getControllerKey(pros::controller_digital_e_t button) {
    uint8_t key = 0;
    switch (button) {
        case pros::E_CONTROLLER_DIGITAL_A: key = AKey; break;
        case pros::E_CONTROLLER_DIGITAL_B: key = BKey; break;
        case pros::E_CONTROLLER_DIGITAL_X: key = XKey; break;
        case pros::E_CONTROLLER_DIGITAL_Y: key = YKey; break;
        case pros::E_CONTROLLER_DIGITAL_UP: key = UpKey; break;
        case pros::E_CONTROLLER_DIGITAL_DOWN: key = DownKey; break;
        case pros::E_CONTROLLER_DIGITAL_LEFT: key = LeftKey; break;
        case pros::E_CONTROLLER_DIGITAL_RIGHT: key = RightKey; break;
        case pros::E_CONTROLLER_DIGITAL_L1: key = L1Key; break;
        case pros::E_CONTROLLER_DIGITAL_L2: key = L2Key; break;
        case pros::E_CONTROLLER_DIGITAL_R1: key = R1Key; break;
        case pros::E_CONTROLLER_DIGITAL_R2: key = R2Key; break;
    }
    return key;
}

uint8_t ControllerValues::getControllerKey(pros::controller_analog_e_t joystick) {
    uint8_t key = 0;

    switch (joystick) {
        case pros::E_CONTROLLER_ANALOG_LEFT_Y: key = LeftYKey; break;
        case pros::E_CONTROLLER_ANALOG_RIGHT_Y: key = RightYKey; break;
        case pros::E_CONTROLLER_ANALOG_LEFT_X: key = LeftXKey; break;
        case pros::E_CONTROLLER_ANALOG_RIGHT_X: key = RightXKey; break;
    }

    return key;
}

Gamepad::Gamepad() {}

Gamepad::Gamepad(pros::controller_id_e_t controllerID, std::vector<std::string> modes) {
    prosController = std::make_shared<pros::Controller>(controllerID);

    if (modes.size() > 0) { this->modes = modes; }

    pros::controller_digital_e_t buttons[] = {
        pros::E_CONTROLLER_DIGITAL_A,    pros::E_CONTROLLER_DIGITAL_B,     pros::E_CONTROLLER_DIGITAL_X,
        pros::E_CONTROLLER_DIGITAL_Y,    pros::E_CONTROLLER_DIGITAL_UP,    pros::E_CONTROLLER_DIGITAL_DOWN,
        pros::E_CONTROLLER_DIGITAL_LEFT, pros::E_CONTROLLER_DIGITAL_RIGHT, pros::E_CONTROLLER_DIGITAL_L1,
        pros::E_CONTROLLER_DIGITAL_L2,   pros::E_CONTROLLER_DIGITAL_R1,    pros::E_CONTROLLER_DIGITAL_R2};

    std::function<int(int)> junkFunc = nullptr;

    std::pair<std::function<int(int)>, std::function<int(int)>> junkFuncs = {nullptr, nullptr};

    for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
        buttonsToFunctions.emplace_back(
            std::make_unique<ButtonMapping>(ButtonMapping(buttons[i], "DEFAULT", junkFuncs)));
    }

    pros::controller_analog_e_t joysticks[] = {pros::E_CONTROLLER_ANALOG_LEFT_Y, pros::E_CONTROLLER_ANALOG_RIGHT_Y,
                                               pros::E_CONTROLLER_ANALOG_LEFT_X, pros::E_CONTROLLER_ANALOG_RIGHT_X};

    for (int i = 0; i < sizeof(joysticks) / sizeof(joysticks[0]); i++) {
        joysticksToFunctions.emplace_back(new JoystickMapping(joysticks[i], "DEFAULT", junkFunc));
    }
}

Gamepad::Gamepad(std::shared_ptr<pros::Controller> prosController, std::vector<std::string> modes) {
    this->prosController = prosController;

    if (modes.size() > 0) { this->modes = modes; }

    pros::controller_digital_e_t buttons[] = {
        pros::E_CONTROLLER_DIGITAL_A,    pros::E_CONTROLLER_DIGITAL_B,     pros::E_CONTROLLER_DIGITAL_X,
        pros::E_CONTROLLER_DIGITAL_Y,    pros::E_CONTROLLER_DIGITAL_UP,    pros::E_CONTROLLER_DIGITAL_DOWN,
        pros::E_CONTROLLER_DIGITAL_LEFT, pros::E_CONTROLLER_DIGITAL_RIGHT, pros::E_CONTROLLER_DIGITAL_L1,
        pros::E_CONTROLLER_DIGITAL_L2,   pros::E_CONTROLLER_DIGITAL_R1,    pros::E_CONTROLLER_DIGITAL_R2};

    pros::controller_analog_e_t joysticks[] = {pros::E_CONTROLLER_ANALOG_LEFT_Y, pros::E_CONTROLLER_ANALOG_RIGHT_Y,
                                               pros::E_CONTROLLER_ANALOG_LEFT_X, pros::E_CONTROLLER_ANALOG_RIGHT_X};

    std::function<int(int)> junkFunc = nullptr;

    std::pair<std::function<int(int)>, std::function<int(int)>> junkFuncs = {nullptr, nullptr};

    for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
        buttonsToFunctions.emplace_back(
            std::make_unique<ButtonMapping>(ButtonMapping(buttons[i], "DEFAULT", junkFuncs)));
    }

    for (int i = 0; i < sizeof(joysticks) / sizeof(joysticks[0]); i++) {
        joysticksToFunctions.emplace_back(new JoystickMapping(joysticks[i], "DEFAULT", junkFunc));
    }
}

Gamepad::~Gamepad() {}

bool Gamepad::startMainLoop() {
    pros::Task task {[this] {
        while (true) {
            autoButtonFunctions();
            pros::delay(20);
        }
    }};

    return true;
}

void Gamepad::autoButtonFunctions() {
    // for (int i = 0; i < buttonStates.size(); i++) {
    //     buttonStates.at(buttonsToFunctions.at(i)->getButton()) =
    //     prosController->get_digital(buttonsToFunctions.at(i)->getButton());
    // }

    for (std::unique_ptr<lemlib::ButtonMapping>& i : buttonsToFunctions) {
        if (getButton({i->getButton()})) { // If buttons are pressed
            i->runFunction(currentMode, prosController->get_digital(i->getButton())); // Runs the function
        }
    }

    for (std::unique_ptr<lemlib::JoystickMapping>& i : joysticksToFunctions) {
        i->runFunction(currentMode, prosController->get_analog(i->getJoystick()));
    }
}

/*================ BUTTONS/JOYSTICK ================*/

bool Gamepad::getButton(std::vector<pros::controller_digital_e_t> buttons) {
    // If all buttons are pressed, then return false will never run, and the function will return true once for loop
    // exists. If any button is pressed, then return false will run. Used in auto button functions.
    for (int i = 0; i > buttons.size(); i++) {
        if (prosController->get_digital(buttons.at(i))) {
            continue;
        } else {
            return false;
        }
    }
    return true;
}

bool Gamepad::newButtonPress(pros::controller_digital_e_t button) {
    bool buttonState = prosController->get_digital(button);
    bool buttonStateLastTick = buttonStates[button];

    bool diffState = false;

    if (buttonState != buttonStateLastTick) {
        diffState = true;
    }

    else {
        diffState = false;
    }

    if (diffState && (buttonState == true)) {
        buttonStates[button] = buttonState;
        return true;
    }

    else {
        buttonStates[button] = buttonState;
        return false;
    }
}

bool Gamepad::toggleButton(pros::controller_digital_e_t button) {

    static int toggleStates[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    if (newButtonPress(button)) { toggleStates[button - 6]++; }

    // - 6 offsets the button int value to match the array index
    if (toggleStates[button - 6] % 2 == 0) {
        return true;
    }

    else {
        return false;
    }
}

int Gamepad::getJoystick(pros::controller_analog_e_t whichJoystick) {
    int joystickValue = prosController->get_analog(whichJoystick);
    return joystickValue;
}

void Gamepad::setFuncToAction(std::pair<std::function<int(int)>, std::function<int(int)>> functionPtr,
                              pros::controller_digital_e_t button, const std::string& mode) {
    std::pair<pros::controller_digital_e_t, std::pair<std::function<int(int)>, std::function<int(int)>>> buttonFuncPair(
        button, functionPtr);

    std::cout << "Controller Key: " << controllerValues.getControllerKey(button) << std::endl;
    buttonsToFunctions.at(controllerValues.getControllerKey(button))->addModeAndFunction(mode, functionPtr);
}

void Gamepad::setFuncToAction(std::function<int(int)> function, pros::controller_analog_e_t joystick,
                              const std::string& mode) {
    std::pair<pros::controller_analog_e_t, std::function<int(int)>> buttonFuncPair(joystick, function);

    std::cout << "Controller Key: " << controllerValues.getControllerKey(joystick) << std::endl;

    // - 12 because the joysticks are ahead of all the buttons, but the vector is only 4 joysticks.
    joysticksToFunctions.at(controllerValues.getControllerKey(joystick) - 12)->addModeAndFunction(mode, function);
}

/*================ MODES ================*/

void Gamepad::addMode(const std::string& mode) { modes.push_back(mode); }

const std::vector<std::string> Gamepad::getModes() { return modes; }

void Gamepad::changeMode(const std::string& mode) { currentMode = mode; }

/*================ MISC. ================*/

std::shared_ptr<pros::Controller> Gamepad::getController() { return prosController; }

void Gamepad::rumble(const char* pattern) { prosController->rumble(pattern); }

std::vector<std::unique_ptr<ButtonMapping>>& Gamepad::getButtonsToFunctions() { return buttonsToFunctions; }

std::vector<std::unique_ptr<JoystickMapping>>& Gamepad::getJoysticksToFunctions() { return joysticksToFunctions; }

} // namespace lemlib