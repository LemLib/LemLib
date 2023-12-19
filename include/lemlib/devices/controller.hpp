#include "main.h"

namespace lemlib {

    struct LEMButtonMapping {
    private:

        pros::controller_digital_e_t button;
        std::vector<std::pair<std::string, int(*)()>> functions;

    public:

        LEMButtonMapping(pros::controller_digital_e_t buttonParam, std::string modeParam, int(*functionParam)()) {
            button = buttonParam;
            functions.push_back({modeParam, functionParam});
        }

        // Acts like a tag
        pros::controller_digital_e_t getButton() {
            return button;
        }

        void addModeAndFunction(std::string modeParam, int(*functionParam)()) {
            functions.push_back({modeParam, functionParam});
        }

        void runFunction(std::string mode) {
            for (int i = 0; i < functions.size(); i++) {
                if (functions[i].first == mode) {
                    functions[i].second();
                }
            }
        }


    };


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
    void setFuncToButton(int(*functionPtr)(), pros::controller_digital_e_t button, std::string modeParam = "DEFAULT");


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

    


};



}