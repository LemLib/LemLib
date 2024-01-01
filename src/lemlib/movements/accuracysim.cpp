
#include "lemlib/movements/accuracysim.hpp"
#include "lemlib/logger/logger.hpp"
#include <charconv>
#include <utility>

namespace lemlib {

    AccuracySIM::AccuracySIM(LEMController* controllerParam, Odom* odomParam, const asset& path, InputReader* inputReaderParam) {
        controller = controllerParam;
        odom = odomParam;
        inputReader = inputReaderParam;

        static constexpr int circleRadius = 6; // In inches? Probably?

        // Stolen from the Pure Pursuit code <3
        std::string input(reinterpret_cast<char*>(path.buf), path.size);
        std::vector<std::string> lines = splitString(input, "\n");
        for (const std::string& line : lines) { // loop through all lines
            if (line == "endData" || line == "endData\r") break;
            std::vector<std::string> pointInput = splitString(line, ", "); // parse line
            const float x = std::stof(pointInput.at(0)); // x position
            const float y = std::stof(pointInput.at(1)); // y position
            const float speed = std::stof(pointInput.at(2)); // speed
            this->circles.emplace_back(x, y, circleRadius); // save data
        }

        currentPathIndex = 0;

    }

    AccuracySIM::~AccuracySIM() {
        delete controller;
        delete odom;
        delete inputReader;
    }

    void AccuracySIM::countdown() {
        controller->rumble(". . ."); // Rumble the controller to let the driver know that the simulation is starting
    }

    void AccuracySIM::markTime(int circleIndex) {
        times.emplace_back(pros::millis() - lastTimeEnteredCircle);
        lastTimeEnteredCircle = pros::millis();

    }

    void AccuracySIM::logData(std::vector<float>* timeVector, std::vector<float>* distanceVector) {
        
        std::cout << " Times: ";

        for (int i = 0; i < timeVector->size(); i++) {
            lemlib::infoSink()->info("{}, ", timeVector->at(i));
        }

        std::cout << " Distances: ";

        for (int i = 0; i < distanceVector->size(); i++) {
            lemlib::infoSink()->info("{}, ", distanceVector->at(i));
        }
      

    }   
    
    bool AccuracySIM::canMoveToNextCircle() {
        
        static constexpr int radiusRange = 6;

        bool isCloseEnough = false;

        if (sqrt(pow(circles.at(currentPathIndex).getX() - botDimensions.getX(), 2) + 
            pow(circles.at(currentPathIndex).getY() - botDimensions.getY(), 2)) 
            < circles.at(currentPathIndex).getRadius() / radiusRange) {

                isCloseEnough = true;
        }

        return isCloseEnough;
    }
    

    void AccuracySIM::recordMaxDistance() {
        
        static float maxDistance = 0;

        float xcirclecord = circles.at(0).getX();
        float ycirclecord = circles.at(0).getY();

        // Distance formula
        float actualCurrentDistance = sqrt(pow(xcirclecord - botDimensions.getX(), 2) + pow(ycirclecord - botDimensions.getY(), 2));
        
        if (actualCurrentDistance > maxDistance) {
            maxDistance = actualCurrentDistance;
        }

        maxDistances.at(currentPathIndex) = maxDistance;

    }

    void AccuracySIM::runSimulation() {
        
        countdown();

        // Start main loop.
        pros::Task task{[=] {

            // While the current circle is NOT past the last circle
            while (currentPathIndex != circles.size()) {
                
                botDimensions.setX(odom->getPose().x);
                botDimensions.setY(odom->getPose().y);

                // Write down the max distance from the circle first, in case the driver passed the center this tick.
                recordMaxDistance();

                // If the distance from the circle is less than 1/3 of the radius, go to the next circle.
                if (canMoveToNextCircle()) {
                    markTime(currentPathIndex);
                    maxDistances.emplace_back(0);
                    currentPathIndex++;

                }          
                
                // If up, down, X, and Y are pressed, reset.
                if (controller->getButton({pros::E_CONTROLLER_DIGITAL_B, pros::E_CONTROLLER_DIGITAL_Y, pros::E_CONTROLLER_DIGITAL_UP, pros::E_CONTROLLER_DIGITAL_DOWN})) {
                    restart();
                }
                
                pros::delay(20);

            }

            logData(&times, &maxDistances);

        
        }};
        
    }

    void AccuracySIM::restart() {
        times.clear();
        maxDistances.clear();
        lastTimeEnteredCircle = 0;
        runSimulation();

    }

    std::pair<float, float> AccuracySIM::getPercentagePerformance() {
        std::pair<float, float> performance;

        float distanceSum = 0;

        for (int i = 0; i > maxDistances.size(); i++) {
            // Divide the max distance by the radius of the circle. 1 means perfect performance.
            distanceSum += maxDistances.at(i) / circles.at(i).getRadius();
        }

        distanceSum /= circles.size();

        performance.first = distanceSum;

        return performance;

    }


}