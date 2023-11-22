/**
 * @file include/lemlib/pid.hpp
 * @author Lemlib Team
 * @brief FAPID class header
 * @version 0.4.5
 * @date 2023-01-15
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <string>
#include "pros/rtos.hpp"
#include "lemlib/units.hpp"
#include "lemlib/util.hpp"

namespace lemlib {
/**
 * @brief Feedforward, Acceleration, Proportional, Integral, Derivative PID controller
 *
 * The controller does not loop on its own. It must be called in a loop.
 * For example: while(!controller.settled) { controller.update(input, output); }
 *
 */
template <isQuantity Q> class FAPID {
    public:
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
        FAPID(float kF, float kA, float kP, float kI, float kD, std::string name, Q base = Q(1.0)) {
            this->kF = kF;
            this->kA = kA;
            this->kP = kP;
            this->kI = kI;
            this->kD = kD;
            this->name = name;
            this->base = base;
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
        void setGains(float kF, float kA, float kP, float kI, float kD) {
            this->kF = kF;
            this->kA = kA;
            this->kP = kP;
            this->kI = kI;
            this->kD = kD;
        }

        /**
         * @brief Set the exit conditions
         *
         * @param largeError range where error is considered large
         * @param smallError range where error is considered small
         * @param largeTime time in milliseconds t
         * @param smallTime
         * @param maxTime
         */
        void setExit(Q largeError, Q smallError, Time largeTime, Time smallTime, Time maxTime) {
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
         * @param log whether to check the most recent terminal input for user input. Default is false because logging
         * multiple PIDs could slow down the program.
         * @return float - output
         */
        float update(Q target, Q position, bool log = false) {
            // check most recent input if logging is enabled
            // this does not run by default because the mutexes could slow down the program
            // calculate output
            Q error = target - position;
            Q deltaError = error - prevError;
            float output = (kF * target + kP * error + kI * totalError + kD * deltaError).convert(base); // todo test
            if (kA != 0) output = slew(output, prevOutput, kA);
            prevOutput = output;
            prevError = error;
            totalError += error;

            return output;
        }

        /**
         * @brief Reset the FAPID
         */
        void reset() {
            prevError = Q(0);
            totalError = Q(0);
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
        bool settled() {
            if (startTime == 0_sec) { // if maxTime has not been set
                startTime = pros::millis() * ms;
                return false;
            } else { // check if the FAPID has settled
                if (pros::millis() * ms - startTime > maxTime) return true; // maxTime has been exceeded
                if (units::abs(prevError) < largeError) { // largeError within range
                    if (largeTimeCounter == 0_sec)
                        largeTimeCounter = pros::millis() * ms; // largeTimeCounter has not been set
                    else if (pros::millis() * ms - largeTimeCounter > largeTime)
                        return true; // largeTime has been exceeded
                }
                if (units::abs(prevError) < smallError) { // smallError within range
                    if (smallTimeCounter == 0_sec)
                        smallTimeCounter = pros::millis() * ms; // smallTimeCounter has not been set
                    else if (pros::millis() * ms - smallTimeCounter > smallTime)
                        return true; // smallTime has been exceeded
                }
                // if none of the exit conditions have been met
                return false;
            }
        }

        /**
         * @brief initialize the FAPID logging system
         *
         * if this function is called, std::cin will be used to interact with the FAPID
         *
         * the user can interact with the FAPID through the terminal
         * the user can access gains and other variables with the following format:
         * <name>.<variable> to get the value of the variable
         * <name>.<variable>_<value> to set the value of the variable
         * for example:
         * pid.kP_0.5 will set the kP value to 0.5
         * list of variables thats value can be set:
         * kF, kA, kP, kI, kD
         * list of variables that can be accessed:
         * kF, kA, kP, kI, kD, totalError
         * list of functions that can be called:
         * reset()
         */
        static void init() {
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
    private:
        float kF;
        float kP;
        float kI;
        float kD;
        float kA;

        Q largeError;
        Q smallError;
        Time largeTime = 0_sec;
        Time smallTime = 0_sec;
        Time maxTime = FOREVER;

        Time largeTimeCounter = 0_sec;
        Time smallTimeCounter = 0_sec;
        Time startTime = 0_sec;

        Q prevError = Q(0);
        Q totalError = Q(0);
        Q base;
        float prevOutput = 0;

        void log() {
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
                        std::cout << totalError.raw() << std::endl;
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

        std::string name;
        static std::string input;
        static pros::Task* logTask;
        static pros::Mutex logMutex;
};

} // namespace lemlib
