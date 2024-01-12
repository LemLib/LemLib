#pragma once

#include "lemlib/chassis/chassis.hpp"
#include "main.h"
#include "lemlib/devices/gamepad.hpp"
#include "lemlib/chassis/differential.hpp"

namespace lemlib {
    
class InputReader {
private:

    Gamepad* Controller; // Pointer to the LEM Controller
    Drivetrain* drivetrain;

    bool useJoyFunctions = false;
    
    static constexpr uint16_t inputMapLength = 3000; // Number of ticks to save inputs for. After the tick limit is reached, the 
        // inputs will no longer be saved.
    static constexpr uint8_t inputSourceNum = 16; // The number of input sources per section of the input map.
    static constexpr uint16_t msWaitTime = 50;

    static constexpr uint8_t LeftY = 0;
    static constexpr uint8_t RightY = 1;
    static constexpr uint8_t LeftX = 2;
    static constexpr uint8_t RightX = 3;
    static constexpr uint8_t A = 4;
    static constexpr uint8_t B = 5;
    static constexpr uint8_t X = 6;
    static constexpr uint8_t Y = 7;
    static constexpr uint8_t Up = 8;
    static constexpr uint8_t Down = 9;
    static constexpr uint8_t Left = 10;
    static constexpr uint8_t Right = 11;
    static constexpr uint8_t L1 = 12;
    static constexpr uint8_t L2 = 13;
    static constexpr uint8_t R1 = 14;
    static constexpr uint8_t R2 = 15;

    std::vector<LEMButtonMapping> buttonsToFunctions; 
    std::vector<int(*)(int)> joystickFunctions;

public:

    static int16_t readInputsMap[inputSourceNum][inputMapLength]; // Stores all inputs

    /**
     * @brief Construct a new Input Reader. It can playback your driving runs and make them repeatable.
     * 
     * @param controllerArg Gamepad pointer.
     * @param buttonsToFunctions The SAME EXACT BUTTONS TO FUNCTIONS VECTOR FROM THE CONTROLLER.    
     * @param joystickFunctions A vector of functions that use the joystick values to run the chassis. MUST
     * be in a CERTAIN ORDER : LeftY, RightY, LeftX, RightX.
     */
    InputReader(Gamepad* controllerArg, std::vector<LEMButtonMapping> buttonsToFunctions, std::vector<int(*)(int)> joystickFunctions);

    /**
     * @brief Construct a new Input Reader. It can playback your driving runs and make them repeatable.
     * 
     * @param controllerArg Gamepad pointer.
     * @param drivetrain The drivetrain to run the joystick functions on. RUNS ON TANK DRIVE.
     * @param buttonsToFunctions The SAME EXACT BUTTONS TO FUNCTIONS VECTOR FROM THE CONTROLLER. 
     */
    InputReader(Gamepad* controllerArg, Drivetrain* drivetrain, std::vector<LEMButtonMapping> buttonsToFunctions);

    ~InputReader();

    void startMainLoop(FILE* fp);

protected:

    static int16_t readInputsBuffer[inputSourceNum * inputMapLength]; // The buffer between the read inputs and a save file.
   
    /**
     * @brief Waits for a certain amount of time. 100% accurate to the microsecond, to prevent any slight errors.
     * 
     * @param globalTick 
     * @param intervalToWait 
     */
    void waitTime(int globalTick, uint16_t intervalToWait);  
    
    /**
     * @brief Replays the inputs.
     * 
     * @param globalTick The global tick, used to determine where to begin playback.
     */
    void replayInputs(uint16_t globalTick = 0);
   
    /**
     * @brief Writes the inputs to the SD Card.
     * 
     * @param fp A pointer to the file to write to.
     */
    void writeInputsToSD(FILE* fp);
   
    /**
     * @brief Loads the inputs from the SD Card.
     * 
     * @param fp A pointer to the file to read from.
     */
    void loadInputsFromSD(FILE* fp);
   
    /**
     * @brief Saves the inputs from the controller at any given tick. 
     * 
     * @param globalTick 
     * @param Controller1 
     */
    void inputsIntoMap(int globalTick); // Saves inputs from controller into the map.
   
    /**
     * @brief Saves a random junk input map, only for debugging.
     * 
     */
    void quickSaver(); 
   

};

}