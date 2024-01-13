#include "lemlib/devices/gamepad.hpp"
#include "pros/misc.h"
#include <utility>
#include <iostream>
#include <vector>

namespace lemlib {

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
        buttonsToFunctions.emplace_back(new LEMButtonMapping(buttons[i], "DEFAULT", junkFuncs));
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
        buttonsToFunctions.emplace_back(new LEMButtonMapping(buttons[i], "DEFAULT", junkFuncs));
    }

    for (int i = 0; i < sizeof(joysticks) / sizeof(joysticks[0]); i++) {
        joysticksToFunctions.emplace_back(new LEMJoystickMapping(joysticks[i], "DEFAULT", junkFunc));
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

std::vector<LEMButtonMapping*> Gamepad::getButtonsToFunctions() { return buttonsToFunctions; }

} // namespace lemlib