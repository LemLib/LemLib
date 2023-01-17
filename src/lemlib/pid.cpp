/**
 * @file src/lemlib/pid.cpp
 * @author Liam Teale
 * @brief FAPID class member definitions
 * @version 0.1.0
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <iostream>
#include <math.h>
#include "lemlib/pid.hpp"
#include "lemlib/util.hpp"



/**
 * @brief Construct a new FAPID
 * 
 * @param kF feedfoward gain, multiplied by target and added to output. Set 0 if disabled
 * @param kA acceleration gain, limits the change in output. Set 0 if disabled
 * @param kP proportional gain, multiplied by error and added to output
 * @param kI integral gain, multiplied by total error and added to output
 * @param kD derivative gain, multiplied by change in error and added to output
 * @param name name of the FAPID. Used for logging
 */
lemlib::FAPID::FAPID(float kF, float kA, float kP, float kI, float kD, std::string name)
{
    this->kF = kF;
    this->kA = kA;
    this->kP = kP;
    this->kI = kI;
    this->kD = kD;
    this->name = name;
}


/**
 * @brief Set gains
 * 
 * @param kF feedfoward gain, multiplied by target and added to output. Set 0 if disabled
 * @param kA acceleration gain, limits the change in output. Set 0 if disabled
 * @param kP proportional gain, multiplied by error and added to output
 * @param kI integral gain, multiplied by total error and added to output
 * @param kD derivative gain, multiplied by change in error and added to output
 */
void lemlib::FAPID::setGains(float kF, float kA, float kP, float kI, float kD)
{
    this->kF = kF;
    this->kA = kA;
    this->kP = kP;
    this->kI = kI;
    this->kD = kD;
}


/**
 * @brief Set the exit conditions
 * 
 * @param largeError 
 * @param smallError 
 * @param largeTime 
 * @param smallTime 
 * @param maxTime 
 */
void lemlib::FAPID::setExit(float largeError, float smallError, int largeTime, int smallTime, int maxTime)
{
    this->largeError = largeError;
    this->smallError = smallError;
    this->largeTime = largeTime;
    this->smallTime = smallTime;
    this->maxTime = maxTime;
}


/**
 * @brief Update the FAPID
 * 
 * @param target the target value
 * @param position the current value
 * @return float - output
 */
float lemlib::FAPID::update(float target, float position)
{
    float error = target - position;
    float deltaError = error - prevError;
    float output = lemlib::slew(kF * target + kP * error + kI * totalError + kD * deltaError, prevOutput, kA);
    prevOutput = output;
    prevError = error;
    totalError += error;
    return output;
}


/**
 * @brief Reset the FAPID
 */
void lemlib::FAPID::reset()
{
    prevError = 0;
    totalError = 0;
    prevOutput = 0;
}


/**
 * @brief Check if the FAPID has settled
 *
 * If the exit conditions have not been set, this function will always return false
 * 
 * @return true - the FAPID has settled
 * @return false - the FAPID has not settled
 */
bool lemlib::FAPID::settled()
{
    if (maxTime == -1) { // if maxTime has not been set
        startTime = pros::c::millis();
        return false;
    } else { // check if the FAPID has settled
        if (pros::c::millis() - startTime > maxTime) // maxTime has been exceeded
        {
            return true;
        }
        if (std::fabs(prevError) < largeError) // largeError within range
        {
            if (!largeTimeCounter) // largeTimeCounter has not been set
            {
                largeTimeCounter = pros::c::millis();
            }
            else if (pros::c::millis() - largeTimeCounter > largeTime) // largeTime has been exceeded
            {
                return true;
            } 
        }
        if (std::fabs(prevError) < smallError) // smallError within range
        {
            if (!smallTimeCounter) // smallTimeCounter has not been set
            {
                smallTimeCounter = pros::c::millis();
            }
            else if (pros::c::millis() - smallTimeCounter > smallTime) // smallTime has been exceeded
            {
                return true;
            } 
        }
        // if none of the exit conditions have been met
        return false;
    }
}


/**
 * @brief Enable logging
 * the user can interact with the FAPID through the terminal
 * the user can access gains and other variables with the following format:
 * <name>.<variable> to get the value of the variable
 * <name>.<variable>_<value> to set the value of the variable
 * for example:z
 * pid.kP_0.5 will set the kP value to 0.5
 * list of variables thats value can be set:
 * kF, kA, kP, kI, kD
 * list of variables that can be accessed:
 * kF, kA, kP, kI, kD, totalError
 * list of functions that can be called:
 * reset()
 */
void lemlib::FAPID::init()
{
    if (logTask != nullptr)
    {
        logTask = new pros::Task{[=] {
            while (true)
            {
                // get input
                std::cin >> input;
                pros::delay(20);
            }
        }};   
    }
}



/**
 * @brief Log the FAPID
 */
void lemlib::FAPID::log()
{
    // check if the input starts with the name of the FAPID
    // try to obtain the logging mutex
    if (logMutex.take(10)) {
        if (input.find(name) == 0) {
            // remove the name from the input
            input.erase(0, name.length() + 1);
            // check if the input is a function
            if (input == "reset()")
            {
                reset();
            }
            else if (input == "kF")
            {
                std::cout << kF << std::endl;
            }
            else if (input == "kA")
            {
                std::cout << kA << std::endl;
            }
            else if (input == "kP")
            {
                std::cout << kP << std::endl;
            }
            else if (input == "kI")
            {
                std::cout << kI << std::endl;
            }
            else if (input == "kD")
            {
                std::cout << kD << std::endl;
            }
            else if (input == "totalError")
            {
                std::cout << totalError << std::endl;
            }
            // check if the input is a variable with a value
            else if (input.find("kF_") == 0)
            {
                input.erase(0, 3);
                kF = std::stof(input);
            }
            else if (input.find("kA_") == 0)
            {
                input.erase(0, 3);
                kA = std::stof(input);
            }
            else if (input.find("kP_") == 0)
            {
                input.erase(0, 3);
                kP = std::stof(input);
            }
            else if (input.find("kI_") == 0)
            {
                input.erase(0, 3);
                kI = std::stof(input);
            }
            else if (input.find("kD_") == 0)
            {
                input.erase(0, 3);
                kD = std::stof(input);
            }
            // clear the input
            input = "";
        }
        // release the logging mutex
        logMutex.give();
    }
}
