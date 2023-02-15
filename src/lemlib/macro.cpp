#include "lemlib/macro.hpp"
#include "logger.hpp"
#include "pros/misc.h"
#include "pros/misc.hpp"
#include <string>

/**
* @brief Get the sequence of buttons
* 
* @return std::initializer_list<pros::controller_digital_e_t> 
*/
std::initializer_list<pros::controller_digital_e_t> lemlib::ControllerSequence::getSequence() {
    return this->sequence;
}

/**
* @brief Get the string representation of the sequence
*
* @return std::string
*/
std::string lemlib::ControllerSequence::toString(pros::Controller controller) {
    std::string str = "";
    for (pros::controller_digital_e_t button : this->sequence) {
        std::cout << button << std::endl;
        str += controller.get_digital(button) ? "1" : "0";
    }
    return str;
}

/**
* @brief Create a new Macro
* 
* @param sequence - the sequence of buttons that will trigger the macro
* @param callback - the function that will be called when the sequence is pressed
*/
lemlib::macro::Macro::Macro(std::initializer_list<pros::controller_digital_e_t>, void (*trigger)(), void (*release)()) {
    this->sequence = sequence;
    this->trigger = trigger;
    this->release = release;
}

/**
* @brief Create a new Macro
* 
* @param sequence - the sequence of buttons that will trigger the macro
* @param trigger - the function that will be called when the sequence is pressed
*/
lemlib::macro::Macro::Macro(std::initializer_list<pros::controller_digital_e_t>, /* inline callback function */ void (*trigger)()) {
    this->sequence = sequence;
    this->trigger = trigger;
    this->release = nullptr;
}

/**
* @brief Check if the sequence is pressed, and run the callback if it is
* 
* @param controller - the controller to check for the sequence
*/
void lemlib::macro::Macro::check(pros::Controller controller) {
    bool fired = true;
    
    int i = 0;

    std::cout << this->sequence.size() << std::endl;

    for (pros::controller_digital_e_t button : this->sequence) {
        std::cout << i << std::endl;        

        if (controller.get_digital(button) == 0) fired = false;
    
        i++;
    }
    lemlib::logger::debug(std::to_string(fired));

    if (fired) {
        if (!this->isThreaded()) this->trigger();
        else pros::Task task(this->trigger);
    } else {
        if (this->release != nullptr) this->release();
    }
}

/**
* @brief Create a new MacroManager
* 
* @param macros - the macros to manage
*/
lemlib::macro::MacroManager::MacroManager(std::initializer_list<Macro> macros) {
    this->macros = macros;

    lemlib::logger::debug("Created MacroManager with " + std::to_string(this->macros.size()) + " macro" + (this->macros.size() == 1 ? "" : "s") + ".");
}

/**
* @brief Check all of the macros
* 
* @param controller - the controller to check for the sequences
*/
void lemlib::macro::MacroManager::check(pros::Controller controller) {
    for (Macro macro : this->macros) macro.check(controller);
}

/**
* @brief Get the macros
* 
* @return std::initializer_list<Macro> 
*/
std::initializer_list<lemlib::macro::Macro> lemlib::macro::MacroManager::getMacros() {
    return this->macros;
}

/**
* @brief Clear all of the macros
* 
* @return bool - whether or not the macros were cleared
*/
bool lemlib::macro::MacroManager::clear() {
    this->macros = {};

    return this->macros.size() == 0;
}