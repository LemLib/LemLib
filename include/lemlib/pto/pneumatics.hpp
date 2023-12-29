/**
 * @file include/lemlib/pto/pneumatics.hpp
 * @author LemLib Team
 * @brief Pneumatics class declarations.
 * @version 0.5.0
 * @date 2023-12-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "pros/adi.hpp"

namespace lemlib {
/**
 * @brief Piston group class
 *
 */
class PistonGroup {
    public:
        // Piston Group class constructor (adi digital outs)
        PistonGroup(const std::vector<pros::ADIDigitalOut> pistonsVec);

        // Piston Group class constructor (ports)
        PistonGroup(const std::vector<uint8_t> ports);

        void extend();
        void retract();
        void toggle();
        
        std::vector<bool> are_extended();

    private:
        std::vector<pros::ADIDigitalOut> pistonsVec;
        std::vector<bool> states;
        int size = 0;
};
} // namespace lemlib