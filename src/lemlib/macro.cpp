#include "lemlib/macro.hpp"

/**
* @brief Get the sequence of buttons
* 
* @return std::initializer_list<pros::controller_digital_e_t> 
*/
std::initializer_list<pros::controller_digital_e_t> lemlib::ControllerSequence::getSequence() {
    return this->sequence;
}

/**
* @brief Create a new Macro
* 
* @param sequence - the sequence of buttons that will trigger the macro
* @param callback - the function that will be called when the sequence is pressed
*/
lemlib::macro::Macro::Macro(lemlib::ControllerSequence sequence, void (*trigger)(), void (*release)()) {
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
lemlib::macro::Macro::Macro(lemlib::ControllerSequence sequence, /* inline callback function */ void (*trigger)()) {
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
    
    for (pros::controller_digital_e_t button : this->sequence.getSequence()) {
        if (!controller.get_digital(button)) {
            fired = false;
            break;
        }
    }

    if (fired) {
        if (!this->isThreaded()) this->trigger();
        else {
            pros::Task task(this->trigger);
        }
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