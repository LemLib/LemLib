#include "pros/device.hpp"
#include "unordered_map"
#include "pros/misc.h"
#include <unordered_map>

namespace lemlib {

struct LEMButtonMapping {
    private:
        pros::controller_digital_e_t button;
        std::vector<std::pair<std::string, std::pair<int (*)(int), int (*)(int)>>> functions;
    public:
        LEMButtonMapping(pros::controller_digital_e_t buttonParam, std::string modeParam,
                         std::pair<int (*)(int), int (*)(int)> functionParam) {
            button = buttonParam;
            functions.push_back({modeParam, functionParam});
        }

        // Acts like a tag.
        pros::controller_digital_e_t getButton() { return button; }

        void addModeAndFunction(std::string modeParam, std::pair<int (*)(int), int (*)(int)> functionParam) {
            functions.push_back({modeParam, functionParam});
        }

        void runFunction(std::string mode, bool buttonState, int funcParam = 0) {
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
};

struct LEMJoystickMapping {
    private:
        pros::controller_analog_e_t joystick;
        std::vector<std::pair<std::string, int (*)(int)>> functions;
    public:
        LEMJoystickMapping(pros::controller_analog_e_t joystickParam, std::string modeParam,
                           int (*functionParam)(int)) {
            joystick = joystickParam;
            functions.push_back({modeParam, functionParam});
        }

        // Acts like a tag
        pros::controller_analog_e_t getJoystick() { return joystick; }

        void addModeAndFunction(std::string modeParam, int (*functionParam)(int)) {
            functions.push_back({modeParam, functionParam});
        }

        void runFunction(std::string mode, int joystickValue) {
            // Loops through all the functions and runs the one that matches the mode.

            for (int i = 0; i < functions.size(); i++) {
                if (functions.at(i).first == mode) { // If the mode matches,
                    if (functions.at(i).second != nullptr) { // If the function is not a null pointer
                        functions.at(i).second(joystickValue); // Run the function pointer
                    }
                }
            }
        }
};

struct LEMControllerValues {
    public:
        static constexpr uint8_t AKey = 0;
        static constexpr uint8_t BKey = 1;
        static constexpr uint8_t XKey = 2;
        static constexpr uint8_t YKey = 3;
        static constexpr uint8_t UpKey = 4;
        static constexpr uint8_t DownKey = 5;
        static constexpr uint8_t LeftKey = 6;
        static constexpr uint8_t RightKey = 7;
        static constexpr uint8_t L1Key = 8;
        static constexpr uint8_t L2Key = 9;
        static constexpr uint8_t R1Key = 10;
        static constexpr uint8_t R2Key = 11;
        static constexpr uint8_t LeftYKey = 12;
        static constexpr uint8_t RightYKey = 13;
        static constexpr uint8_t LeftXKey = 14;
        static constexpr uint8_t RightXKey = 15;

        LEMControllerValues() {}

        uint8_t getControllerKey(pros::controller_digital_e_t button) {
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

        uint8_t getControllerKey(pros::controller_analog_e_t joystick) {
            uint8_t key = 0;

            switch (joystick) {
                case pros::E_CONTROLLER_ANALOG_LEFT_Y: key = LeftYKey; break;
                case pros::E_CONTROLLER_ANALOG_RIGHT_Y: key = RightYKey; break;
                case pros::E_CONTROLLER_ANALOG_LEFT_X: key = LeftXKey; break;
                case pros::E_CONTROLLER_ANALOG_RIGHT_X: key = RightXKey; break;
            }

            return key;
        }
};

class Gamepad {
    private:
    protected:
        pros::Controller* prosController;

        std::string currentMode = "DEFAULT";

        std::vector<std::string> modes = {"DEFAULT"};
        std::vector<LEMButtonMapping*> buttonsToFunctions;
        std::vector<LEMJoystickMapping*> joysticksToFunctions;

        std::unordered_map<pros::controller_digital_e_t, bool> buttonStates;

        LEMControllerValues controllerValues;
        /**
         * @brief When the main loop starts, this function gets called and uses function pointers/modes info to run
         * user-made functions automaically.
         *
         */
        void autoButtonFunctions();

        Gamepad();
    public:
        /**
         * @brief Construct a new Gamepad object
         *
         * @param controllerID
         * @param modesParam
         */
        Gamepad(pros::controller_id_e_t controllerID, std::vector<std::string> modesParam = {"DEFAULT"});

        /**
         * @brief Construct a new Gamepad object
         *
         * @param controller
         * @param modesParam
         */
        Gamepad(pros::Controller* controller, std::vector<std::string> modesParam = {"DEFAULT"});

        ~Gamepad();

        bool startMainLoop();

        /*================ BUTTONS/JOYSTICK ================*/

        /**
         * @brief Get whether a button is pressed.
         *
         * @param buttons vector of the buttons you want checked.
         * @return true
         * @return false
         */
        bool getButton(std::vector<pros::controller_digital_e_t> buttons);

        /**
         * @brief Checks if there was a new button press.
         *
         * @param button
         * @return true
         * @return false
         */
        bool newButtonPress(pros::controller_digital_e_t button);

        /**
         * @brief Grabs the value of a button, acting as a toggle. Click once to turn on, it'll return true until you
         * click it again.
         *
         * @param button
         * @return true
         * @return false
         */
        bool toggleButton(pros::controller_digital_e_t button);

        /**
         * @brief Get valaue of a joystick.
         *
         * @param whichJoystick
         * @return int
         */
        int getJoystick(pros::controller_analog_e_t whichJoystick);

        /**
         * @brief Sets a button to two user-made functions. When pressed, the function will automatically run without
         * needing input from the user.
         *
         * @param functionPtr A pair of integer function pointers. Int Data Type lets you return an error code if
         * necessary. First function pointer is when the controller value is false, second function pointer is when the
         * controller value is true.
         * @param button Button you want these functions paired to.
         * @param modeParam Mode to add the button to. Defaults to "DEFAULT".
         */
        void setFuncToAction(std::pair<int (*)(int), int (*)(int)>, pros::controller_digital_e_t button,
                             std::string modeParam = "DEFAULT");

        /**
         * @brief Sets a button to two user-made functions. When pressed, the function will automatically run without
         * needing input from the user.
         *
         * @param functionPtr An integer function pointers. Int Data Type lets you return an error code if necessary.
         * @param joystick Joystick you want these functions paired to.
         * @param modeParam Mode to add the button to. Defaults to "DEFAULT".
         */
        void setFuncToAction(int (*functionPtr)(int), pros::controller_analog_e_t joystick,
                             std::string modeParam = "DEFAULT");

        /*================ MODES ================*/

        /**
         * @brief Adds a mode to the controller. Different modes mean different button mappings. Input a string to add a
         * mode.
         *
         * @param modeParam
         */
        void addMode(std::string modeParam);

        /**
         * @brief Get a vector of all modes.
         *
         * @return std::vector<std::string>
         */
        std::vector<std::string> getModes();

        /**
         * @brief Changes the mode of the controller to something else. Can be automated using the setFuncToButton
         * function and making a function containing changeMode().
         *
         * @param modeParam
         */
        void changeMode(std::string modeParam);

        /*================ MISC. ================*/

        /**
         * @brief Gets the underlying PROS Controller.
         *
         * @return pros::Controller*
         */
        pros::Controller* getController();

        /**
         * @brief Rumble Controller.
         *
         */
        void rumble(const char* pattern);

        std::vector<LEMButtonMapping*> getButtonsToFunctions();
};

} // namespace lemlib