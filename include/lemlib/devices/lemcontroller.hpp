#include "pros/device.hpp"

namespace lemlib {

    struct LEMButtonMapping {
    private:

        pros::controller_digital_e_t button;
        std::vector<std::pair<std::string, int(*)(int)>> functions;

    public:

        LEMButtonMapping(pros::controller_digital_e_t buttonParam, std::string modeParam, int(*functionParam)(int)) {
            button = buttonParam;
            functions.push_back({modeParam, functionParam});
        }

        // Acts like a tag
        pros::controller_digital_e_t getButton() {
            return button;
        }

        void addModeAndFunction(std::string modeParam, int(*functionParam)(int)) {
            functions.push_back({modeParam, functionParam});
        }

        void runFunction(std::string mode, int funcParam = 0) {
            for (int i = 0; i < functions.size(); i++) {
                if (functions[i].first == mode) {
                    functions[i].second(funcParam);
                }
            }
        }

        
    };

    /*struct LEMControllerValues {
    private:

        int16_t LeftY, RightY, LeftX, RightX, A, B, X, Y, Up, Down, Left, Right, L1, L2, R1, R2 = 0;

    public:

        LEMControllerValues() {
            
        }

        static constexpr uint8_t LeftYKey = 0;
        static constexpr uint8_t RightYKey = 1;
        static constexpr uint8_t LeftXKey = 2;
        static constexpr uint8_t RightXKey = 3;
        static constexpr uint8_t AKey = 4;
        static constexpr uint8_t BKey = 5;
        static constexpr uint8_t XKey = 6;
        static constexpr uint8_t YKey = 7;
        static constexpr uint8_t UpKey = 8;
        static constexpr uint8_t DownKey = 9;
        static constexpr uint8_t LeftKey = 10;
        static constexpr uint8_t RightKey = 11;
        static constexpr uint8_t L1Key = 12;
        static constexpr uint8_t L2Key = 13;
        static constexpr uint8_t R1Key = 14;
        static constexpr uint8_t R2Key = 15;

        void setControllerValue(uint8_t key, int16_t value) {
            switch (key) {
                case LeftYKey:
                    LeftY = value;
                    break;
                case RightYKey:
                    RightY = value;
                    break;
                case LeftXKey:
                    LeftX = value;
                    break;
                case RightXKey:
                    RightX = value;
                    break;
                case AKey:
                    A = value;
                    break;
                case BKey:
                    B = value;
                    break;
                case XKey:
                    X = value;
                    break;
                case YKey:
                    Y = value;
                    break;
                case UpKey:
                    Up = value;
                    break;
                case DownKey:
                    Down = value;
                    break;
                case LeftKey:
                    Left = value;
                    break;
                case RightKey:
                    Right = value;
                    break;
                case L1Key:
                    L1 = value;
                    break;
                case L2Key:
                    L2 = value;
                    break;
                case R1Key:
                    R1 = value;
                    break;
                case R2Key:
                    R2 = value;
                    break;
            }
        }

    };*/


class LEMController {
private:

protected:

    pros::Controller* prosController;

    LEMController();

    std::string currentMode = "DEFAULT";

    std::vector<std::string> modes = {"DEFAULT"};
    std::vector<LEMButtonMapping> buttonsToFunctions; 


    /**
     * @brief When the main loop starts, this function gets called and uses function pointers/modes info to run user-made functions automaically.
     * 
     */
    void autoButtonFunctions();

public:

    LEMController(pros::controller_id_e_t controllerID, std::vector<std::string> modesParam = {"DEFAULT"});
    
    LEMController(pros::Controller* controller, std::vector<std::string> modesParam = {"DEFAULT"});
    
    ~LEMController();

    bool startMainLoop();

    /*================ BUTTONS/JOYSTICK ================*/

    /**
     * @brief Get whether a button is pressed.
     * 
     * @param button 
     * @return true 
     * @return false 
     */
    bool getButton(pros::controller_digital_e_t button);

    /**
     * @brief Checks if two buttons were pressed at once.
     * 
     * @param button 
     * @param button2 
     * @return true 
     * @return false 
     */
    bool getButtonCombination(pros::controller_digital_e_t button, pros::controller_digital_e_t button2);

    /**
     * @brief Checks if three buttons were pressed at once.
     * 
     * @param button 
     * @param button2 
     * @param button3 
     * @return true 
     * @return false 
     */
    bool getButtonCombination(pros::controller_digital_e_t button, pros::controller_digital_e_t button2, pros::controller_digital_e_t button3);

    /**
     * @brief Checks if there was a new button press.
     * 
     * @param button 
     * @return true 
     * @return false 
     */
    bool newButtonPress(pros::controller_digital_e_t button);

    /**
     * @brief Grabs the value of a button, acting as a toggle. Click once to turn on, it'll return true until you click it again.
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
     * @brief Sets a button to a user-made function. When pressed, the function will automatically run without needing input from the user.
     * 
     * @param functionPtr An integer function pointer. Int Data Type lets you return an error code if necessary.
     * @param button 
     * @param modeParam Mode to add the button to. Defaults to "DEFAULT".
     */
    void setFuncToButton(int(*functionPtr)(int), pros::controller_digital_e_t button, std::string modeParam = "DEFAULT");


    /*================ MODES ================*/

    /**
     * @brief Adds a mode to the controller. Different modes mean different button mappings. Input a string to add a mode.
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
     * @brief Changes the mode of the controller to something else. Can be automated using the setFuncToButton function and making a function containing changeMode().
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

    std::vector<LEMButtonMapping> getButtonsToFunctions();

    


};



}