/**
 * @file macro.hpp
 * @author LemLib
 * @brief Macros for the V5 Controller in teleop
 * @version 0.4.1
 * @date 2023-02-13
 * 
 * @copyright Copyright (c) 2023
 */

#include "pros/misc.hpp"
#include <initializer_list>
#include <iostream>

namespace lemlib {
    namespace macro {

        /**
         * @brief A sequence of controller buttons
         */
        class ControllerSequence {
            public:
            /**
             * @brief Create a new ControllerSequence
             * 
             * @tparam sequence - the buttons in the sequence, in order
             */
            template<pros::controller_digital_e_t... sequence>
            ControllerSequence();

            /**
             * @brief Get the sequence of buttons
             * 
             * @return std::initializer_list<pros::controller_digital_e_t> 
             */
            std::initializer_list<pros::controller_digital_e_t> getSequence();
            private:
            std::initializer_list<pros::controller_digital_e_t> sequence;
        };

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
                 * @param callback - the function that will be called when the sequence is pressed
                 */
                Macro(ControllerSequence sequence, /* inline callback function */ void (*callback)());

            /**
             * @brief Check if the sequence is pressed, and run the callback if it is
             * 
             * @param controller - the controller to check for the sequence
             */
            void run(pros::Controller controller);

            private:
                ControllerSequence sequence;
                void (*callback)();
        };

        class MacroManager {
            public:
                /**
                 * @brief Create a new MacroManager
                 * 
                 * @param macros - the macros to manage
                 */
                MacroManager(std::initializer_list<Macro> macros);

                /**
                 * @brief Run all of the macros
                 * 
                 * @param controller - the controller to check for the sequences
                 */
                void run(pros::Controller controller);

                /**
                 * @brief Add a macro to the manager
                 * 
                 * @param macro - the macro to add
                 */
                void addMacro(Macro macro);

                /**
                 * @brief Remove a macro from the manager
                 * 
                 * @param macro - the macro to remove
                 */
                void removeMacro(Macro macro);

                /**
                 * @brief Get the macros
                 * 
                 * @return std::initializer_list<Macro> 
                 */
                std::initializer_list<Macro> getMacros();

                /**
                 * @brief Clear all of the macros
                 * 
                 * @return bool - whether or not the macros were cleared
                 */
                bool clearMacros();
            private:
                std::initializer_list<Macro> macros;
        };
    }
}