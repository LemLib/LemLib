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
#include <set>
#include "pros/imu.h"
#include "pros/rtos.hpp"
#include "lemlib/units.hpp"
#include "lemlib/util.hpp"

namespace lemlib {
struct Gains {
        float kF = 0;
        float kA = 0;
        float kP = 0;
        float kI = 0;
        float kD = 0;
};

/** @brief A function taking the target and the (target, Gains) elements adjacent to it, computing the resulting gains
 */
template <isQuantity Q> using Interpolator = std::function<Gains(Q, std::pair<Q, Gains>, std::pair<Q, Gains>)>;

/**
 * @brief A gain interpolator that selects the gains with the closest target
 *
 * @param target the target at which to interpolate
 * @param below the lower adjacent (target, Gains) value
 * @param above the higher adjacent (target, Gains) value
 *
 * @returns the interpolated gains
 */
template <isQuantity Q> Gains interpolateNearest(Q target, std::pair<Q, Gains> below, std::pair<Q, Gains> above) {
    if (units::abs(target - below.first) < units::abs(target - above.first)) {
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
template <isQuantity Q> Gains interpolateLinear(Q target, std::pair<Q, Gains> below, std::pair<Q, Gains> above) {
    Gains nearest = lemlib::interpolateNearest(target, below, above);
    auto [x1, y1] = below;
    auto [x2, y2] = above;

    // Should kF and kA be interpolated?
    // It seems that feedforward constants should remain the same

    float kF = nearest.kF;
    float kA = nearest.kA;

    float kP = lemlib::linearInterp(target, x1, y1.kP, x2, y2.kP);
    float kI = lemlib::linearInterp(target, x1, y1.kI, x2, y2.kI);
    float kD = lemlib::linearInterp(target, x1, y1.kD, x2, y2.kD);

    return Gains {kF, kA, kP, kI, kD};
}

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
        FAPID(float kF, float kA, float kP, float kI, float kD, std::string name, Q base = Q(1.0))
            : currentGains(Gains {kF, kA, kP, kI, kD}),
              name(name),
              base(base) {}

        /**
         * @brief Construct a new FAPID
         *
         * @param Gains the Gains for the FAPID to use
         * @param name name of the FAPID. Used for logging
         */
        FAPID(Gains gains, std::string name, Q base = Q(1.0))
            : currentGains(gains),
              name(name),
              base(base) {};

        /**
         * @brief Construct a new FAPID
         *
         * @param Gains the default Gains for the FAPID to use
         * @param scheduled a set of (target, Gains) pairs to use for gain scheduling
         * @param name name of the FAPID. Used for logging
         */
        FAPID(Gains gains, std::set<std::pair<float, Gains>> scheduled, std::string name, Q base = Q(1.0))
            : currentGains(gains),
              scheduledGains(scheduled),
              name(name),
              base(base) {};

        /**
         * @brief Construct a new FAPID
         *
         * @param Gains the default Gains for the FAPID to use
         * @param scheduled a set of (target, Gains) pairs to use for gain scheduling
         * @param interpolator the function to use when interpolating Gains when scheduling
         * @param name name of the FAPID. Used for logging
         */
        FAPID(Gains gains, std::set<std::pair<float, Gains>> scheduled, Interpolator<Q> interpolator, std::string name)
            : currentGains(gains),
              scheduledGains(scheduled),
              gainInterpolator(interpolator),
              name(name),
              base(base) {};

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
            this->currentGains.kF = kF;
            this->currentGains.kA = kA;
            this->currentGains.kP = kP;
            this->currentGains.kI = kI;
            this->currentGains.kD = kD;
        }

        /**
         * @brief Set gains
         *
         * @param Gains the new gains
         */
        void setGains(Gains gains) { this->currentGains = gains; }

        /**
         * @brief Set scheduled gains
         *
         * @param Gains the new scheduled gains
         */
        void setScheduledGains(std::set<std::pair<float, Gains>> scheduled);

        /**
         * @brief Set gain interpolator
         *
         * @param Gains the new gain interpolator
         */
        void setGainInterpolator(Interpolator<Q> interpolator);

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
            Q deltaError = error - this->prevError;
            float output = (this->currentGains.kF * target + this->currentGains.kP * error +
                            this->currentGains.kI * this->totalError + this->currentGains.kD * deltaError)
                               .convert(base); // todo test
            if (this->currentGains.kA != 0) output = slew(output, this->prevOutput, this->currentGains.kA);
            this->prevOutput = output;
            this->prevError = error;
            this->totalError += error;

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
         * the user can access Gains and other variables with the following format:
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
        // An ordered set of (target, Gains) pairs for gain scheduling
        std::set<std::pair<float, Gains>> scheduledGains = {};

        // The Gains the PID will use
        Gains currentGains;

        // A function taking the target and the elements adjacent to the target in the (target, gain) set, computing the
        // final gains
        Interpolator<Q> gainInterpolator = interpolateNearest<Q>;

        Q previousTarget = Q(0);
        Q largeError;
        Q smallError;
        Time largeTime = 0_sec;
        Time smallTime = 0_sec;
        Time maxTime = FOREVER; // -1 means no max time set, run forever

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
                        std::cout << this->currentGains.kF << std::endl;
                    } else if (input == "kA") {
                        std::cout << this->currentGains.kA << std::endl;
                    } else if (input == "kP") {
                        std::cout << this->currentGains.kP << std::endl;
                    } else if (input == "kI") {
                        std::cout << this->currentGains.kI << std::endl;
                    } else if (input == "kD") {
                        std::cout << this->currentGains.kD << std::endl;
                    } else if (input == "totalError") {
                        std::cout << totalError.raw() << std::endl;
                    } else if (input.find("kF_") == 0) {
                        input.erase(0, 3);
                        this->currentGains.kF = std::stof(input);
                    } else if (input.find("kA_") == 0) {
                        input.erase(0, 3);
                        this->currentGains.kA = std::stof(input);
                    } else if (input.find("kP_") == 0) {
                        input.erase(0, 3);
                        this->currentGains.kP = std::stof(input);
                    } else if (input.find("kI_") == 0) {
                        input.erase(0, 3);
                        this->currentGains.kI = std::stof(input);
                    } else if (input.find("kD_") == 0) {
                        input.erase(0, 3);
                        this->currentGains.kD = std::stof(input);
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
