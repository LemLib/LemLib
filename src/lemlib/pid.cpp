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

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
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
lemlib::FAPID::FAPID(float kF, float kA, float kP, float kI, float kD, std::string name) {
    currentGains = Gains {kF, kA, kP, kI, kD};
    this->name = name;
}

/**
 * @brief Construct a new FAPID
 *
 * @param gains the gains for the FAPID to use
 * @param name name of the FAPID. Used for logging
 */
lemlib::FAPID::FAPID(Gains gains, std::string name) {
    currentGains = gains;
    this->name = name;
}

/**
 * @brief Construct a new FAPID
 *
 * @param gains the default gains for the FAPID to use
 * @param scheduled a set of (target, Gains) pairs to use for gain scheduling
 * @param name name of the FAPID. Used for logging
 */
lemlib::FAPID::FAPID(Gains gains, std::set<std::pair<float, Gains>> scheduled, std::string name) {
    currentGains = gains;
    scheduledGains = scheduled;
    this->name = name;
}

/**
 * @brief Construct a new FAPID
 *
 * @param gains the default gains for the FAPID to use
 * @param scheduled a set of (target, Gains) pairs to use for gain scheduling
 * @param interpolator the function to use when interpolating gains when scheduling
 * @param name name of the FAPID. Used for logging
 */
lemlib::FAPID::FAPID(Gains gains, std::set<std::pair<float, Gains>> scheduled, Interpolator interpolator,
                     std::string name) {
    currentGains = gains;
    scheduledGains = scheduled;
    gainInterpolator = interpolator;
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
void lemlib::FAPID::setGains(float kF, float kA, float kP, float kI, float kD) {
    auto gains = Gains {kF, kA, kP, kI, kD};
    currentGains = gains;
}

/**
 * @brief Set gains
 *
 * @param gains the new gains
 */
void lemlib::FAPID::setGains(Gains gains) { currentGains = gains; }

/**
 * @brief Set scheduled gains
 *
 * @param gains the new scheduled gains
 */
void lemlib::FAPID::setScheduledGains(std::set<std::pair<float, Gains>> scheduled) { scheduledGains = scheduled; }

/**
 * @brief Set gain interpolator
 *
 * @param gains the new gain interpolator
 */
void lemlib::FAPID::setGainInterpolator(Interpolator interpolator) { gainInterpolator = interpolator; }

/**
 * @brief Set the exit conditions
 *
 * @param largeError
 * @param smallError
 * @param largeTime
 * @param smallTime
 * @param maxTime
 */
void FAPID::setExit(float largeError, float smallError, int largeTime, int smallTime, int maxTime) {
    this->largeError = largeError;
    this->smallError = smallError;
    this->largeTime = largeTime;
    this->smallTime = smallTime;
    this->maxTime = maxTime;
}

/**
 * @brief A gain interpolator that selects the gains with the closest target
 *
 * @param target the target at which to interpolate
 * @param below the lower adjacent (target, Gains) value
 * @param above the higher adjacent (target, Gains) value
 *
 * @returns the interpolated gains
 */
lemlib::Gains interpolateNearest(float target, std::pair<float, Gains> below, std::pair<float, Gains> above) {
    if (std::abs(target - below.first) < std::abs(target - above.first)) {
        return below.second;
    } else {
        return above.second;
    }
}

/**
 * @brief A gain interpolator that linearly interpolates gains
 *
 * @param target the target at which to interpolate
 * @param below the lower adjacent (target, Gains) value
 * @param above the higher adjacent (target, Gains) value
 *
 * @returns the interpolated gains
 */
lemlib::Gains interpolateLinear(float target, std::pair<float, Gains> below, std::pair<float, Gains> above) {
    auto nearest = lemlib::interpolateNearest(target, below, above);
    auto [x1, y1] = below;
    auto [x2, y2] = above;

    // Should kF and kA be interpolated?
    // It seems that feedforward constants should remain the same

    auto kF = nearest.kF;
    auto kA = nearest.kA;

    auto kP = lemlib::linearInterp(target, x1, y1.kP, x2, y2.kP);
    auto kI = lemlib::linearInterp(target, x1, y1.kI, x2, y2.kI);
    auto kD = lemlib::linearInterp(target, x1, y1.kD, x2, y2.kD);

    return Gains {kF, kA, kP, kI, kD};
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
float lemlib::FAPID::update(float target, float position, bool log) {
    // Check if we have scheduled gains, and if we have a different target
    if (!scheduledGains.empty() && previousTarget != target) {
        auto upper = std::upper_bound(scheduledGains.begin(), scheduledGains.end(), target,
                                      [](auto value, auto entry) { return value < entry.first; });

        // Nearest scheduled gains above and below (or equal) the target
        auto above = *upper;
        auto below = upper == scheduledGains.begin() ? *upper : *(--upper);
        currentGains = gainInterpolator(target, below, above);
    }

    // check most recent input if logging is enabled
    // this does not run by default because the mutexes could slow down the program
    // calculate output

    auto [kF, kA, kP, kI, kD] = currentGains;
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
    if (startTime == 0) { // if maxTime has not been set
        startTime = pros::c::millis();
        return false;
    } else { // check if the FAPID has settled
        if (pros::c::millis() - startTime > maxTime) return true; // maxTime has been exceeded
        if (std::fabs(prevError) < largeError) { // largeError within range
            if (!largeTimeCounter) largeTimeCounter = pros::c::millis(); // largeTimeCounter has not been set
            else if (pros::c::millis() - largeTimeCounter > largeTime) return true; // largeTime has been exceeded
        }
        if (std::fabs(prevError) < smallError) { // smallError within range
            if (!smallTimeCounter) smallTimeCounter = pros::c::millis(); // smallTimeCounter has not been set
            else if (pros::c::millis() - smallTimeCounter > smallTime) return true; // smallTime has been exceeded
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
                std::cout << currentGains.kF << std::endl;
            } else if (input == "kA") {
                std::cout << currentGains.kA << std::endl;
            } else if (input == "kP") {
                std::cout << currentGains.kP << std::endl;
            } else if (input == "kI") {
                std::cout << currentGains.kI << std::endl;
            } else if (input == "kD") {
                std::cout << currentGains.kD << std::endl;
            } else if (input == "totalError") {
                std::cout << totalError << std::endl;
            } else if (input.find("kF_") == 0) {
                input.erase(0, 3);
                currentGains.kF = std::stof(input);
            } else if (input.find("kA_") == 0) {
                input.erase(0, 3);
                currentGains.kA = std::stof(input);
            } else if (input.find("kP_") == 0) {
                input.erase(0, 3);
                currentGains.kP = std::stof(input);
            } else if (input.find("kI_") == 0) {
                input.erase(0, 3);
                currentGains.kI = std::stof(input);
            } else if (input.find("kD_") == 0) {
                input.erase(0, 3);
                currentGains.kD = std::stof(input);
            }
            // clear the input
            input = "";
        }
        // release the logging mutex
        logMutex.give();
    }
}
}; // namespace lemlib