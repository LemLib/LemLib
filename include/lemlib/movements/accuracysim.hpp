
#include "lemlib/movements/inputreader.hpp"
#include "lemlib/shapesncoords.hpp"

std::string testString;

namespace lemlib {

    class AccuracySIM {
    private:

        int amountOfSkillsSimsDone; // The amount of skills simulations that have been done. Used for saving the data to the
            // file.

        LEMController* controller; // Pointer to the controller driver is using 

        AutonSelector* autonSelector; // A pointer to the auton selector, so you can pick which auton is being run

        InputReader* inputReader; // A pointer to the input reader, to track misinputs

        Logger* logger; // Logger for the saving the simulation results

        Poller* poller; // Poller for getting past simulation results

        std::vector<Circle> circles; // Vector of circles that will be used for the performance evaluation

        std::vector<float> times; // Vector of times that the driver took to completely go through each circle

        std::vector<float> maxDistances; // Vector of the maximum distance that the driver was from the current targeted circle

        float lastTimeEnteredCircle; // The time that the driver last entered a circle, used for calculating the time it took
            // to go through the circle. Recorded in milliseconds.

        Rectangle botDimensions; // Rectangle with the dimensions of the bot. Be as accurate as possible, as this
            // will be used for the performance evaluation. Only measure relevant things: an axle sticking out of the bot
            // or a random piece of metal that doesn't affect the bot's performance doesn't need to be included. However,
            // if you have a mechanism that extends out of the bot, that should be included.
        
        /**
        * @brief The regular "3, 2, 1" from the controller when starting a skills run
        * 
        */
        void countdown(); 

        /**
        * @brief Mark the time when the driver goes through a circle
        * 
        * @param circleIndex Which circle the driver is going through, as an index for the vector of circles
        */
        void markTime(int circleIndex); 

        /**
        * @brief Save the data to the SD card. Things like the times for each circle, percent performance.
        * 
        */
        void saveData(); // Save the data to the SD card

        /**
         * @brief Records the distance from the current targeted circle over time, and sets it to the vector of max distances.
         * 
         */
        void recordMaxDistance(); // Record the distance from the last circle

        
        


    public:

        /**
        * @brief Construct a new Skills Simulation object
        * 
        * @param controllerArg Pointer to the controller the driver is using
        * @param autonSelectorArg Pointer to the auton selector that can be either inputted or not. If not inputted, the 
        * auton will not be run and by default driver control will run.
        * @param inputReaderArg Pointer to the input reader that can be either inputted or not. If not inputted, the
        */
        AccuracySIM(LEMController* controllerArg, AutonSelector* autonSelectorArg = nullptr, InputReader* inputReaderArg = nullptr);

        /**
        * @brief Destroy the Skills Simulation object
        * 
        */
        ~AccuracySIM();

        /**
        * @brief Begin Simulating
        * 
        */
        void runSimulation();

        /**
        * @brief Restart the simulation
        * 
        */
        void restart();  // Probably will be tied to a button on the controller

        /**
        * @brief Get the Percentage Performance of the driver in the simulation
        * 
        * @return Range 
        */
        std::pair<float, float> getPercentagePerformance();


    };

}