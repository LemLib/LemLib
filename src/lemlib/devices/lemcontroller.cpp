#include "lemlib/devices/lemcontroller.hpp"
#include "pros/misc.h"
#include <utility>
#include <vector>

namespace lemlib {

LEMController::LEMController() {
    
}

LEMController::LEMController(pros::controller_id_e_t controllerID, std::vector<std::string> modesParam) {

    prosController = new pros::Controller(controllerID);

    if (modesParam.size() > 0) {
        modes = modesParam;
    }    

    pros::controller_digital_e_t buttons[] = {pros::E_CONTROLLER_DIGITAL_A, 
        pros::E_CONTROLLER_DIGITAL_B, pros::E_CONTROLLER_DIGITAL_X, 
        pros::E_CONTROLLER_DIGITAL_Y, pros::E_CONTROLLER_DIGITAL_UP, 
        pros::E_CONTROLLER_DIGITAL_DOWN, pros::E_CONTROLLER_DIGITAL_LEFT, 
        pros::E_CONTROLLER_DIGITAL_RIGHT, pros::E_CONTROLLER_DIGITAL_L1, 
        pros::E_CONTROLLER_DIGITAL_L2, pros::E_CONTROLLER_DIGITAL_R1, 
        pros::E_CONTROLLER_DIGITAL_R2};

    for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++) {
        buttonsToFunctions.emplace_back(buttons[i], "DEFAULT", [](){});
        
    }
    
}

LEMController::LEMController(pros::Controller* controller, std::vector<std::string> modesParam) {

    prosController = controller;

    if (modesParam.size() > 0) {
        modes = modesParam;
    }    

    pros::controller_digital_e_t buttons[] = {pros::E_CONTROLLER_DIGITAL_A, 
        pros::E_CONTROLLER_DIGITAL_B, pros::E_CONTROLLER_DIGITAL_X, 
        pros::E_CONTROLLER_DIGITAL_Y, pros::E_CONTROLLER_DIGITAL_UP, 
        pros::E_CONTROLLER_DIGITAL_DOWN, pros::E_CONTROLLER_DIGITAL_LEFT, 
        pros::E_CONTROLLER_DIGITAL_RIGHT, pros::E_CONTROLLER_DIGITAL_L1, 
        pros::E_CONTROLLER_DIGITAL_L2, pros::E_CONTROLLER_DIGITAL_R1, 
        pros::E_CONTROLLER_DIGITAL_R2};

    int (*junkFunc)(int) = [](int x){return 0;};

    std::pair<int(*)(int), int(*)(int)> junkFuncs = {nullptr, nullptr };

    for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++) {
        buttonsToFunctions.emplace_back( new LEMButtonMapping( buttons[i], "DEFAULT", junkFuncs ) );
        
    }
    
}

LEMController::~LEMController() {
    delete prosController;
    for (int i = 0; i < buttonsToFunctions.size(); i++) {
        delete &buttonsToFunctions.at(i);
    }
}

bool LEMController::startMainLoop() {

    pros::Task task{[=] {

        while (true) {
            autoButtonFunctions();
            pros::delay(20);
        }
    
    }};

    return true;

}

void LEMController::autoButtonFunctions() {
    
    for (int i = 0; i < buttonsToFunctions.size(); i++) {
        if (getButton( {buttonsToFunctions.at(i)->getButton()} )) { // If buttons are pressed
            buttonsToFunctions.at(i)->runFunction(currentMode, controllerValues.getControllerKey(buttonsToFunctions.at(i)->getButton())); // Runs the function
        }
    }    
    
}

/*================ BUTTONS/JOYSTICK ================*/

bool LEMController::getButton(std::vector<pros::controller_digital_e_t> buttons) {
    
    // If all buttons are pressed, then return false will never run, and the function will return true once for loop exists. 
    // If any button is pressed, then return false will run.
    for (int i = 0; i > buttons.size(); i++) {
        if (prosController->get_digital(buttons.at(i))) {
            continue;
        }
        else {
            return false;
        }
    }
    return true;
}


bool LEMController::newButtonPress(pros::controller_digital_e_t button) {

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

bool LEMController::toggleButton(pros::controller_digital_e_t button) {

    static int toggleState = 0;

    if (newButtonPress(button)) {
        toggleState++;
    }
    
    if (toggleState % 2 == 0) {
        return true;
    }

    else {
        return false;
    }

}

int LEMController::getJoystick(pros::controller_analog_e_t whichJoystick) {
    int joystickValue = prosController->get_analog(whichJoystick);
    return joystickValue;
}

void LEMController::setFuncToButton(std::pair<int(*)(int), int(*)(int)> functionPtr, pros::controller_digital_e_t button, std::string modeParam) {

    std::pair<pros::controller_digital_e_t, std::pair<int(*)(int), int(*)(int)>> buttonFuncPair(button, functionPtr);

    buttonsToFunctions.at(controllerValues.getControllerKey(button))->addModeAndFunction(modeParam, functionPtr);
    

}

/*================ MODES ================*/

void LEMController::addMode(std::string modeParam) {
    modes.push_back(modeParam);
}

std::vector<std::string> LEMController::getModes() {
    return modes;
}

void LEMController::changeMode(std::string modeParam) {
    currentMode = modeParam;

}

/*================ MISC. ================*/

pros::Controller* LEMController::getController() {
    return prosController;
}

void LEMController::rumble(const char* pattern) {
    prosController->rumble(pattern);
}

std::vector<LEMButtonMapping*> LEMController::getButtonsToFunctions() {
    return buttonsToFunctions;
}

}