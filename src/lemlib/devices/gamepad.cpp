#include "lemlib/devices/gamepad.hpp"
#include "pros/misc.h"
#include <utility>
#include <iostream>
#include <vector>

namespace lemlib {

ButtonMapping::ButtonMapping(pros::controller_digital_e_t buttonParam, std::string modeParam,
              std::pair<int (*)(int), int (*)(int)> functionParam) {
    button = buttonParam;
    functions.push_back({modeParam, functionParam});
}

// Acts like a tag.
pros::controller_digital_e_t ButtonMapping::getButton() { return button; }

void ButtonMapping::addModeAndFunction(std::string modeParam, std::pair<int (*)(int), int (*)(int)> functionParam) {
    functions.push_back({modeParam, functionParam});
}

void ButtonMapping::runFunction(std::string mode, bool buttonState, int funcParam) {
    // Loops through all the functions and runs the one that matches the mode.

    for (int i = 0; i < functions.size(); i++) {
        if (functions.at(i).first == mode) { // If the mode matches,

            if (buttonState == true) {
                if (functions.at(i).second.second !=
                    nullptr) { // If the function for when its true is not a null pointer
                    functions.at(i).second.second(funcParam); // Run the function pointer for the true state
                }
            } else {
                if (functions.at(i).second.first !=
                    nullptr) { // If the function for when its false is not a null pointer
                    functions.at(i).second.first(funcParam); // Run the function pointer for the false state
                }
            }
        }
    }
}









JoystickMapping::JoystickMapping(pros::controller_analog_e_t joystickParam, std::string modeParam,
                                 int (*functionParam)(int)) {
    joystick = joystickParam;
    functions.push_back({modeParam, functionParam});
}

// Acts like a tag
pros::controller_analog_e_t JoystickMapping::getJoystick() { return joystick; }

void JoystickMapping::addModeAndFunction(std::string modeParam, int (*functionParam)(int)) {
    functions.push_back({modeParam, functionParam});
}

void JoystickMapping::runFunction(std::string mode, int joystickValue) {
    // Loops through all the functions and runs the one that matches the mode.
    for (int i = 0; i < functions.size(); i++) {
        if (functions.at(i).first == mode) { // If the mode matches,
            if (functions.at(i).second != nullptr) { // If the function is not a null pointer
                functions.at(i).second(joystickValue); // Run the function pointer
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

Gamepad::Gamepad(pros::controller_id_e_t controllerID, std::vector<std::string> modesParam) {
    prosController = new pros::Controller(controllerID);

    if (modesParam.size() > 0) { modes = modesParam; }

    pros::controller_digital_e_t buttons[] = {
        pros::E_CONTROLLER_DIGITAL_A,    pros::E_CONTROLLER_DIGITAL_B,     pros::E_CONTROLLER_DIGITAL_X,
        pros::E_CONTROLLER_DIGITAL_Y,    pros::E_CONTROLLER_DIGITAL_UP,    pros::E_CONTROLLER_DIGITAL_DOWN,
        pros::E_CONTROLLER_DIGITAL_LEFT, pros::E_CONTROLLER_DIGITAL_RIGHT, pros::E_CONTROLLER_DIGITAL_L1,
        pros::E_CONTROLLER_DIGITAL_L2,   pros::E_CONTROLLER_DIGITAL_R1,    pros::E_CONTROLLER_DIGITAL_R2};

    std::pair<int (*)(int), int (*)(int)> junkFuncs = {nullptr, nullptr};

    for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
        buttonsToFunctions.emplace_back(new ButtonMapping(buttons[i], "DEFAULT", junkFuncs));
    }
}

Gamepad::Gamepad(pros::Controller* controller, std::vector<std::string> modesParam) {
    prosController = controller;

    if (modesParam.size() > 0) { modes = modesParam; }

    pros::controller_digital_e_t buttons[] = {
        pros::E_CONTROLLER_DIGITAL_A,    pros::E_CONTROLLER_DIGITAL_B,     pros::E_CONTROLLER_DIGITAL_X,
        pros::E_CONTROLLER_DIGITAL_Y,    pros::E_CONTROLLER_DIGITAL_UP,    pros::E_CONTROLLER_DIGITAL_DOWN,
        pros::E_CONTROLLER_DIGITAL_LEFT, pros::E_CONTROLLER_DIGITAL_RIGHT, pros::E_CONTROLLER_DIGITAL_L1,
        pros::E_CONTROLLER_DIGITAL_L2,   pros::E_CONTROLLER_DIGITAL_R1,    pros::E_CONTROLLER_DIGITAL_R2};

    pros::controller_analog_e_t joysticks[] = {pros::E_CONTROLLER_ANALOG_LEFT_Y, pros::E_CONTROLLER_ANALOG_RIGHT_Y,
                                               pros::E_CONTROLLER_ANALOG_LEFT_X, pros::E_CONTROLLER_ANALOG_RIGHT_X};

    int (*junkFunc)(int) = [](int x) { return 0; };

    std::pair<int (*)(int), int (*)(int)> junkFuncs = {nullptr, nullptr};

    for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
        buttonsToFunctions.emplace_back(new ButtonMapping(buttons[i], "DEFAULT", junkFuncs));
    }

    for (int i = 0; i < sizeof(joysticks) / sizeof(joysticks[0]); i++) {
        joysticksToFunctions.emplace_back(new JoystickMapping(joysticks[i], "DEFAULT", junkFunc));
    }
}

Gamepad::~Gamepad() {
    delete prosController;
    for (int i = 0; i < buttonsToFunctions.size(); i++) { delete &buttonsToFunctions.at(i); }
    for (int i = 0; i < joysticksToFunctions.size(); i++) { delete &joysticksToFunctions.at(i); }
}

bool Gamepad::startMainLoop() {
    pros::Task task {[this] {
        while (true) {
            autoButtonFunctions();
            pros::delay(500);
        }
    }};

    return true;
}

void Gamepad::autoButtonFunctions() {
    // for (int i = 0; i < buttonStates.size(); i++) {
    //     buttonStates.at(buttonsToFunctions.at(i)->getButton()) =
    //     prosController->get_digital(buttonsToFunctions.at(i)->getButton());
    // }

    for (int i = 0; i < buttonsToFunctions.size(); i++) {
        if (getButton({buttonsToFunctions.at(i)->getButton()})) { // If buttons are pressed
            buttonsToFunctions.at(i)->runFunction(
                currentMode, prosController->get_digital(buttonsToFunctions.at(i)->getButton())); // Runs the function
        }
    }

    for (int i = 0; i < joysticksToFunctions.size(); i++) {
        joysticksToFunctions.at(i)->runFunction(currentMode,
                                                prosController->get_analog(joysticksToFunctions.at(i)->getJoystick()));
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
    static int toggleState = 0;

    if (newButtonPress(button)) { toggleState++; }

    if (toggleState % 2 == 0) {
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

void Gamepad::setFuncToAction(std::pair<int (*)(int), int (*)(int)> functionPtr, pros::controller_digital_e_t button,
                              std::string modeParam) {
    std::pair<pros::controller_digital_e_t, std::pair<int (*)(int), int (*)(int)>> buttonFuncPair(button, functionPtr);

    std::cout << "Controller Key: " << controllerValues.getControllerKey(button) << std::endl;
    buttonsToFunctions.at(controllerValues.getControllerKey(button))->addModeAndFunction(modeParam, functionPtr);
}

void Gamepad::setFuncToAction(int (*functionPtr)(int), pros::controller_analog_e_t joystick, std::string modeParam) {
    std::pair<pros::controller_analog_e_t, int (*)(int)> buttonFuncPair(joystick, functionPtr);

    std::cout << "Controller Key: " << controllerValues.getControllerKey(joystick) << std::endl;

    // - 12 because the joysticks are ahead of all the buttons, but the vector is only 4 joysticks.
    joysticksToFunctions.at(controllerValues.getControllerKey(joystick) - 12)
        ->addModeAndFunction(modeParam, functionPtr);
}

/*================ MODES ================*/

void Gamepad::addMode(std::string modeParam) { modes.push_back(modeParam); }

std::vector<std::string> Gamepad::getModes() { return modes; }

void Gamepad::changeMode(std::string modeParam) { currentMode = modeParam; }

/*================ MISC. ================*/

pros::Controller* Gamepad::getController() { return prosController; }

void Gamepad::rumble(const char* pattern) { prosController->rumble(pattern); }

std::vector<ButtonMapping*> Gamepad::getButtonsToFunctions() { return buttonsToFunctions; }

} // namespace lemlib