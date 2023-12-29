/**
 * @file src/lemlib/pto/pneumatics.hpp
 * @author LemLib Team
 * @brief definitions for Pneumatics.
 * @version 0.5.0
 * @date 2023-12-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "lemlib/pto/pneumatics.hpp"

namespace lemlib {
// PistonGroup class constructors
PistonGroup::PistonGroup(const std::vector<pros::ADIDigitalOut> pistonsVec)
    : pistonsVec(pistonsVec) {
    for (pros::ADIDigitalOut piston : pistonsVec) {
        states.push_back(false);
        size += 1;
    }
}

PistonGroup::PistonGroup(const std::vector<uint8_t> ports) {
    for (std::uint8_t port : ports) {
        pistonsVec.push_back(pros::ADIDigitalOut(port));
        states.push_back(false);
        size += 1;
    }
}

// PistonGroup extend
void PistonGroup::extend() {
    for (pros::ADIDigitalOut piston : pistonsVec) {
        piston.set_value(true);
    }
}

// PistonGroup retract
void PistonGroup::retract() {
    for (pros::ADIDigitalOut piston : pistonsVec) {
        piston.set_value(true);
    }
}

// PistonGroup toggle
void PistonGroup::toggle() {
    for (int i = 0; i < size; i++) {
        pros::ADIDigitalOut piston = pistonsVec[i];
        bool isPistonExtended = states[i];
        if (isPistonExtended) piston.set_value(false);
        else piston.set_value(true);
    }
}

// PistonGroup get states
std::vector<bool> PistonGroup::are_extended() {
    return this->states;
}
} // namespace lemlib