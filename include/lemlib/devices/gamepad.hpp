#include "pros/device.hpp"
#include "pros/misc.h"
#include <memory>
#include <unordered_map>

namespace lemlib {

struct ButtonMapping {
    private:
        pros::controller_digital_e_t button;
        std::vector<std::pair<std::string, std::pair<int (*)(int), int (*)(int)>>> functions;
    public:
        ButtonMapping(pros::controller_digital_e_t button, std::string mode,
                      std::pair<int (*)(int), int (*)(int)> function);

        // Acts like a tag.
        pros::controller_digital_e_t getButton();

        void addModeAndFunction(std::string mode, std::pair<int (*)(int), int (*)(int)> function); 

        void runFunction(std::string mode, bool buttonState, int func = 0);
};

struct JoystickMapping {
    private:
        pros::controller_analog_e_t joystick;
        std::vector<std::pair<std::string, int (*)(int)>> functions;
    public:
        JoystickMapping(pros::controller_analog_e_t joystickParam, std::string modeParam, int (*functionParam)(int));

        // Acts like a tag
        pros::controller_analog_e_t getJoystick();

        void addModeAndFunction(std::string modeParam, int (*functionParam)(int));

        void runFunction(std::string mode, int joystickValue);
};

struct ControllerValues {
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

        ControllerValues();

        uint8_t getControllerKey(pros::controller_digital_e_t button);

        uint8_t getControllerKey(pros::controller_analog_e_t joystick);
};

class Gamepad {
    protected:
        std::shared_ptr<pros::Controller> prosController;

        std::string currentMode = "DEFAULT";

        std::vector<std::string> modes = {"DEFAULT"};
        std::vector<std::unique_ptr<ButtonMapping>> buttonsToFunctions;
        std::vector<std::unique_ptr<JoystickMapping>> joysticksToFunctions;

        std::unordered_map<pros::controller_digital_e_t, bool> buttonStates;

        ControllerValues controllerValues;

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
         * @param modes
         */
        Gamepad(pros::controller_id_e_t controllerID, std::vector<std::string> modes = {"DEFAULT"});

        /**
         * @brief Construct a new Gamepad object
         *
         * @param controller
         * @param modes
         */
        Gamepad(std::shared_ptr<pros::Controller> controller, std::vector<std::string> modes = {"DEFAULT"});

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
         * @param mode Mode to add the button to. Defaults to "DEFAULT".
         */
        void setFuncToAction(std::pair<int (*)(int), int (*)(int)>, pros::controller_digital_e_t button,
                             const std::string& mode = "DEFAULT");

        /**
         * @brief Sets a button to two user-made functions. When pressed, the function will automatically run without
         * needing input from the user.
         *
         * @param functionPtr An integer function pointers. Int Data Type lets you return an error code if necessary.
         * @param joystick Joystick you want these functions paired to.
         * @param mode Mode to add the button to. Defaults to "DEFAULT".
         */
        void setFuncToAction(int (*functionPtr)(int), pros::controller_analog_e_t joystick,
                             const std::string& mode = "DEFAULT");

        /*================ MODES ================*/

        /**
         * @brief Adds a mode to the controller. Different modes mean different button mappings. Input a string to add a
         * mode.
         *
         * @param mode
         */
        void addMode(const std::string& mode);

        /**
         * @brief Get a vector of all modes.
         *
         * @return std::vector<std::string>
         */
        const std::vector<std::string> getModes();

        /**
         * @brief Changes the mode of the controller to something else. Can be automated using the setFuncToButton
         * function and making a function containing changeMode().
         *
         * @param mode
         */
        void changeMode(const std::string& mode);

        /*================ MISC. ================*/

        /**
         * @brief Gets the underlying PROS Controller.
         *
         * @return pros::Controller Shared Ptr
         */
        std::shared_ptr<pros::Controller> getController();

        /**
         * @brief Rumble Controller.
         *
         */
        void rumble(const char* pattern);

        std::vector<std::unique_ptr<ButtonMapping>>& getButtonsToFunctions();

        std::vector<std::unique_ptr<JoystickMapping>>& getJoysticksToFunctions();
};

} // namespace lemlib