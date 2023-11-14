/**
 * @file src/lemlib/pid.cpp
 * @author LemLib Team
 * @brief FAPID class member definitions
 * @version 0.4.5
 * @date 2023-01-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>
#include <math.h>
#include "lemlib/pid.hpp"
#include "lemlib/util.hpp"

namespace lemlib {
// define static variables
std::string FAPID::input = "FAPID";
pros::Task* FAPID::logTask = nullptr;
pros::Mutex FAPID::logMutex = pros::Mutex();

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
FAPID::FAPID(float kF, float kA, float kP, float kI, float kD, std::string name) {
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
void FAPID::setGains(float kF, float kA, float kP, float kI, float kD) {
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
void FAPID::setExit(float largeError, float smallError, Time largeTime, Time smallTime, Time maxTime) {
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
 * @param log whether to check the most recent terminal input for user input. Default is false because logging multiple
 * PIDs could slow down the program.
 * @return float - output
 */
float FAPID::update(float target, float position, bool log) {
    // check most recent input if logging is enabled
    // this does not run by default because the mutexes could slow down the program
    // calculate output
    float error = target - position;
    float deltaError = error - prevError;
    float output = kF * target + kP * error + kI * totalError + kD * deltaError;
    if (kA != 0) output = slew(output, prevOutput, kA);
    prevOutput = output;
    prevError = error;
    totalError += error;

    return output;
}

/**
 * @brief Reset the FAPID
 */
void FAPID::reset() {
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
bool FAPID::settled() {
    if (startTime == 0_sec) { // if maxTime has not been set
        startTime = pros::millis() * ms;
        return false;
    } else { // check if the FAPID has settled
        if (pros::millis() * ms - startTime > maxTime) return true; // maxTime has been exceeded
        if (std::fabs(prevError) < largeError) { // largeError within range
            if (largeTimeCounter == 0_sec) largeTimeCounter = pros::millis() * ms; // largeTimeCounter has not been set
            else if (pros::millis() * ms - largeTimeCounter > largeTime) return true; // largeTime has been exceeded
        }
        if (std::fabs(prevError) < smallError) { // smallError within range
            if (smallTimeCounter == 0_sec) smallTimeCounter = pros::millis() * ms; // smallTimeCounter has not been set
            else if (pros::millis() * ms - smallTimeCounter > smallTime) return true; // smallTime has been exceeded
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
void FAPID::init() {
    if (logTask != nullptr) {
        logTask = new pros::Task {[=] {
            while (true) {
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
void FAPID::log() {
    // check if the input starts with the name of the FAPID
    // try to obtain the logging mutex
    if (logMutex.take(5)) {
        if (input.find(name) == 0) {
            // remove the name from the input
            input.erase(0, name.length() + 1);
            // check if the input is a function
            if (input == "reset()") {
                reset();
            } else if (input == "kF") {
                std::cout << kF << std::endl;
            } else if (input == "kA") {
                std::cout << kA << std::endl;
            } else if (input == "kP") {
                std::cout << kP << std::endl;
            } else if (input == "kI") {
                std::cout << kI << std::endl;
            } else if (input == "kD") {
                std::cout << kD << std::endl;
            } else if (input == "totalError") {
                std::cout << totalError << std::endl;
            } else if (input.find("kF_") == 0) {
                input.erase(0, 3);
                kF = std::stof(input);
            } else if (input.find("kA_") == 0) {
                input.erase(0, 3);
                kA = std::stof(input);
            } else if (input.find("kP_") == 0) {
                input.erase(0, 3);
                kP = std::stof(input);
            } else if (input.find("kI_") == 0) {
                input.erase(0, 3);
                kI = std::stof(input);
            } else if (input.find("kD_") == 0) {
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
}; // namespace lemlib