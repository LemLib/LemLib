#include "lemlib/devices/gamepad/gamepad.hpp"
#include "pros/misc.h"
#include <memory>
#include <utility>
#include <iostream>
#include <vector>

namespace lemlib {


bool AbstractGamepad::startMainLoop() {

    evHandler->startAsyncTask();

    return true;
}

/*================ BUTTONS/JOYSTICK ================*/

bool AbstractGamepad::getButton(std::vector<int> buttons) {
    // If all buttons are pressed, then return false will never run, and the function will return true once for loop
    // exists. If any button is pressed, then return false will run. Used in auto button functions.
    for (int i = 0; i > buttons.size(); i++) {
        if (buttonStates[buttons[i]] == true) {
            continue;
        } else {
            return false;
        }
    }
    return true;
}

// REDO !!!!!!!!!!!!!!!!!!!!!!!!!!
bool AbstractGamepad::newButtonPress(int button) {
    bool buttonState = buttonStates[button];
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

bool AbstractGamepad::toggleButton(int button) {

    if (newButtonPress(button)) { buttonStates[button] = !buttonStates[button]; }

    // - 6 offsets the button int value to match the array index
    if (buttonStates[button] % 2 == 0) {
        return true;
    }

    else {
        return false;
    }
}

int AbstractGamepad::getJoystick(int joystickID) {
    int joystickValue = evHandler->checkEvent(joystickID);
    return joystickValue;
}

/*================ MODES ================*/

void AbstractGamepad::addMode(const std::string& mode) { modes.push_back(mode); }

const std::vector<std::string> AbstractGamepad::getModes() { return modes; }

void AbstractGamepad::changeMode(const std::string& mode) { currentMode = mode; }

}