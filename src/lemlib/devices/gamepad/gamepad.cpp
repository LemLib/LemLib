#include "lemlib/devices/gamepad/gamepad.hpp"
#include "pros/misc.h"
#include <memory>
#include <utility>
#include <iostream>
#include <vector>

namespace lemlib {

bool AbstractGamepad::startMainLoop() {
    for (uint id : butHandler->getAllIDs()) { buttonStates[id] = getButton({(int)id}); }
    butHandler->startAsyncTask();

    return true;
}

/*================ BUTTONS/JOYSTICK ================*/

bool AbstractGamepad::getButton(std::vector<int> buttons) {

    for (auto id : buttons) {
        if (!butHandler->isValidID(id)) {
            std::cout << "Invalid Button ID " << id << ": Returning false by default. " << std::endl;
            return false;
        }
    }    

    // If all buttons are pressed, then return false will never run, and the function will return true once for loop
    // exists. If any button is pressed, then return false will run. Used in auto button functions.

    for (uint id : buttons) {
        buttonStates[id] = butHandler->checkEvent(id);
        // std::cout << butHandler->checkEvent(id) << std::endl;
    }

    for (uint id : buttons) {
        if (buttonStates[id] == true) {
            continue;
        } else {
            return false;
        }
    }

    return true;
}

// REDO !!!!!!!!!!!!!!!!!!!!!!!!!!
bool AbstractGamepad::newButtonPress(int button) {
    if (!butHandler->isValidID(button)) {
        std::cout << "Invalid Button ID " << button << ": Returning false by default. " << std::endl;
        return false;
    }

    bool buttonStateLastTick = buttonStates[button];
    bool buttonState = getButton({button});

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

bool AbstractGamepad::toggleButton(int button) {
    if (!butHandler->isValidID(button)) {
        std::cout << "Invalid Button ID " << button << ": Returning false by default. " << std::endl;
        return false;
    }

    bool isButtonPressed = newButtonPress(button);

    if (!buttonToggleStates[button] && isButtonPressed) {
        // std::cout << buttonToggleStates[button] << " <- buttonstates | buttonPress -> " << !newButtonPress(button) <<
        // std::endl;
        buttonToggleStates[button] = true;
        return true;
    } else if (buttonToggleStates[button] && isButtonPressed) {
        buttonToggleStates[button] = false;
        // std::cout << buttonToggleStates[button] << " <- buttonstates | buttonPress -> " << !newButtonPress(button) <<
        // std::endl;
        return false;
    } else if (buttonToggleStates[button] && !isButtonPressed) {
        return true;
    } else if (!buttonToggleStates[button] && !isButtonPressed) {
        return false;
    } else {
        return false;
    }
}

int AbstractGamepad::getJoystick(int joystickID) {
    int joystickValue = joyHandler->checkEvent(joystickID);
    return joystickValue;
}

/*================ MODES ================*/

// void AbstractGamepad::addMode(const std::string& mode) { modes.push_back(mode); }

// const std::vector<std::string> AbstractGamepad::getModes() { return modes; }

// void AbstractGamepad::changeMode(const std::string& mode) { currentMode = mode; }

} // namespace lemlib