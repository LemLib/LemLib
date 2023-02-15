/**
 * @file macro.hpp
 * @author LemLib
 * @brief Macros for the V5 Controller in teleop
 * @version 0.4.1
 * @date 2023-02-13
 * 
 * @copyright Copyright (c) 2023
 */

#include "lemlib/util.hpp"
#include "pros/misc.hpp"
#include "pros/rtos.hpp"

#include <initializer_list>
#include <iostream>

namespace lemlib {
    /**
    * @brief A sequence of controller buttons
    */
    class ControllerSequence {
        public:
        /**
        * @brief Create a new ControllerSequence
        */
        ControllerSequence() {
            this->sequence = {};
        }

        /**
        * @brief Create a new ControllerSequence
        * 
        * @param std::initializer_list<pros::controller_digital_e_t> sequence - the buttons in the sequence, in order
        */
        ControllerSequence(std::initializer_list<pros::controller_digital_e_t> sequence) {
            std::cout << "Creating new ControllerSequence" << std::endl;
            std::cout << "Sequence size: " << sequence.size() << std::endl;

            this->sequence = sequence;
        }

        /**
        * @brief Get the sequence of buttons
        * 
        * @return std::initializer_list<pros::controller_digital_e_t> 
        */
        std::initializer_list<pros::controller_digital_e_t> getSequence();
        
        /**
         * @brief Get the string representation of the sequence
         *
         * @param controller - the controller to get the button values from
         *
         * @return std::string
         */
        std::string toString(pros::Controller controller);

        private:
        std::initializer_list<pros::controller_digital_e_t> sequence;
    };
    namespace macro {

        /**
         * @brief A macro that can be run during teleop
         * 
         */
        class Macro {
            public:
                /**
                 * @brief Create a new Macro
                 * 
                 * @param sequence - the sequence of buttons that will trigger the macro
                 * @param trigger - the function that will be called when the sequence is pressed
                 * @param release - the function that will be called when the sequence is released
                 */
                Macro(std::initializer_list<pros::controller_digital_e_t>, /* inline callback function */ void (*trigger)(), void (*release)());

                /**
                 * @brief Create a new Macro
                 * 
                 * @param sequence - the sequence of buttons that will trigger the macro
                 * @param trigger - the function that will be called when the sequence is pressed
                 */
                 Macro(std::initializer_list<pros::controller_digital_e_t>, /* inline callback function */ void (*trigger)());

                /**
                * @brief Check if the sequence is pressed, and run the callback if it is
                * 
                * @param controller - the controller to check for the sequence
                */
                void check(pros::Controller controller);

                /**
                * @brief Set whether or not the macro should be run in a new thread
                * 
                * @param threaded - new value
                * @return Macro - the current macro
                */
                Macro setThreaded(bool threaded) {
                    this->threaded = threaded; return *this;
                }

                /**
                * @brief Get whether or not the macro should be run in a new thread
                * 
                * @return bool 
                */
                bool isThreaded() { return this->threaded; }

                /**
                 * @brief Get the sequence of buttons
                 * 
                 * @return ControllerSequence 
                 */
                std::initializer_list<pros::controller_digital_e_t> getSequence() { return this->sequence; }
            private:
                std::initializer_list<pros::controller_digital_e_t> sequence;
                bool threaded = false; /* to run in a new thread */
                void (*trigger)();
                void (*release)();
        };

        class MacroManager {
            public:
                /**
                 * @brief Create a new MacroManager
                 */
                MacroManager() {
                    this->macros = {};
                }

                /**
                 * @brief Create a new MacroManager
                 * 
                 * @param macros - the macros to manage
                 */
                MacroManager(std::initializer_list<Macro> macros);

                /**
                 * @brief Check all of the macros
                 * 
                 * @param controller - the controller to check for the sequences
                 */
                void check(pros::Controller controller);

                /**
                 * @brief Get the macros
                 * 
                 * @return std::initializer_list<Macro> 
                 */
                std::initializer_list<Macro> getMacros();

                /**
                 * @brief Set the macros
                 * 
                 * @param macros - the new macros
                 */
                void setMacros(std::initializer_list<Macro> macros) { this->macros = macros; }

                /**
                 * @brief Clear all of the macros
                 * 
                 * @return bool - whether or not the macros were cleared
                 */
                bool clear();
            private:
                std::initializer_list<Macro> macros;
        };
    }
}