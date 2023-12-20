#include "lemlib/devices/controller.hpp"
#include "pros/misc.h"
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

LEMController::~LEMController() {
    delete prosController;
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
        if (getButton(buttonsToFunctions.at(i).getButton())) { // If buttons are pressed
            buttonsToFunctions.at(i).runFunction(currentMode); // Runs the function
        }
    }    
    
}

/*================ BUTTONS/JOYSTICK ================*/

bool LEMController::getButton(pros::controller_digital_e_t button) {
    return prosController->get_digital(button);
}

bool LEMController::getButtonCombination(pros::controller_digital_e_t button, pros::controller_digital_e_t button2) {
    return prosController->get_digital(button) && prosController->get_digital(button2);
}

bool LEMController::getButtonCombination(pros::controller_digital_e_t button, pros::controller_digital_e_t button2, pros::controller_digital_e_t button3) {
    return prosController->get_digital(button) && prosController->get_digital(button2) && prosController->get_digital(button3);
}

bool LEMController::newButtonPress(pros::controller_digital_e_t button) {
    
    static bool buttonState = false;
    static bool buttonStateLastTick = false;

    buttonStateLastTick = buttonState;

    buttonState = prosController->get_digital(button);

    if (buttonState != buttonStateLastTick) {
        return true;
    }

    else {
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

void LEMController::setFuncToButton(int(*functionPtr)(int), pros::controller_digital_e_t button, std::string modeParam) {

    std::pair<pros::controller_digital_e_t, int(*)(int)> buttonFuncPair(button, functionPtr);

    

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

std::vector<LEMButtonMapping> LEMController::getButtonsToFunctions() {
    return buttonsToFunctions;
}

}