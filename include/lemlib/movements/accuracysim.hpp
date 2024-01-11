
#include "lemlib/logger/infoSink.hpp"
#include "lemlib/movements/inputreader.hpp"
#include "lemlib/odom/odom.hpp"
#include "lemlib/shapesncoords.hpp"
#include "lemlib/logger/poller.hpp"
#include "lemlib/logger/logger.hpp"
#include "lemlib/util.hpp"

namespace lemlib {

    class AccuracySIM {
    private:

        int amountOfSkillsSimsDone; // The amount of skills simulations that have been done. Used for saving the data to the
            // file.

        Gamepad* controller; // Pointer to the controller driver is using 

        InputReader* inputReader; // A pointer to the input reader, to track misinputs

        Odom* odom; // Pointer to the odometry, to get the position of the bot
        
        std::vector<Circle> circles; // Vector of circles that will be used for the performance evaluation

        std::vector<float> times; // Vector of times that the driver took to completely go through each circle

        std::vector<float> maxDistances; // Vector of the maximum distance that the driver was from the current targeted circle

        float lastTimeEnteredCircle; // The time that the driver last entered a circle, used for calculating the time it took
            // to go through the circle. Recorded in milliseconds.

        int currentPathIndex; // The index of the current circle that the driver is going through

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
        * @brief Logs the data to the terminal. Things like the times for each circle, percent performance.
        * 
        */
        void logData(std::vector<float>* timeVector, std::vector<float>* distanceVector); // Save the data to the SD card

        /**
         * @brief Load the accuracy history from the SD card
         * 
         */
        void loadAccuracyHistory(); 

        /**
         * @brief Records the distance from the current targeted circle over time, and sets it to the vector of max distances.
         * 
         */
        void recordMaxDistance(); // Record the distance from the last circle

        /**
         * @brief Checks if the current circle index can move onto the next circle. You don't have to pass through the center in fact,
         * only 1/6 of the radius of the circle from the center. 
         */
        bool canMoveToNextCircle(); 


    public:

        /**
        * @brief Construct a new Skills Simulation object
        * 
        * @param controllerArg Pointer to the controller the driver is using
        * @param odomParam Pointer to the odometry. Used for getting the position of the bot
        * @param path The path that the driver will be going through. Imported from /static/
        * @param inputReaderArg Pointer to the input reader that can be either inputted or not. If not inputted, the
        */
        AccuracySIM(Gamepad* controllerArg, Odom* odomParam, const asset& path, InputReader* inputReaderArg = nullptr);

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