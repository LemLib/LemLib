#include "pros/device.hpp"
#include "pros/misc.h"
#include <memory>
#include <unordered_map>

namespace lemlib {

class AbstractGamepad {
    protected:
        
        std::string currentMode = "DEFAULT";
        std::vector<std::string> modes = {"DEFAULT"};

        // std::vector<std::unique_ptr<ButtonMapping>> buttonsToFunctions;
        // std::vector<std::unique_ptr<JoystickMapping>> joysticksToFunctions;

        std::unordered_map<int, bool> buttonStates;
        std::unordered_map<int, int> joystickStates;

        // ControllerValues controllerValues;

        /**
         * @brief When the main loop starts, this function gets called and uses function pointers/modes info to run
         * user-made functions automaically.
         *
         */
        virtual void autoButtonFunctions() = 0;
    public:
        
        bool startMainLoop();

        /*================ BUTTONS/JOYSTICK ================*/

        /**
         * @brief Get whether a button is pressed.
         *
         * @param buttons vector of the buttons you want check. Use button enums inside the vectors.
         * @return true
         * @return false
         */
        bool getButton(std::vector<int> buttons);

        /**
         * @brief Checks if there was a new button press.
         *
         * @param button
         * @return true
         * @return false
         */
        bool newButtonPress(int button);

        /**
         * @brief Grabs the value of a button, acting as a toggle. Click once to turn on, it'll return true until you
         * click it again.
         *
         * @param button
         * @return true
         * @return false
         */
        bool toggleButton(int button);

        /**
         * @brief Get valaue of a joystick.
         *
         * @param whichJoystick
         * @return int
         */
        int getJoystick(int whichJoystick);


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


};

} // namespace lemlib