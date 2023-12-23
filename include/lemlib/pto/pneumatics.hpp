/**
 * @file include/lemlib/pneumatics.hpp
 * @author LemLib Team
 * @brief Pneumatics class declarations
 * @version 0.4.7
 * @date 2023-12-20
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <memory>

#include "pros/adi.hpp"

// #include "lemlib/util.hpp"

namespace lemlib {

class PistonGroup {
    /* Heavily based off of pros::adi::Pneumatics.
       Function names mirror pros::adi::Pneumatics to avoid confusion.
       Thus, `are_extended()` is in snake_case instead of camelCase, unlike the rest of LemLib */
    public:
        // Piston Group class constructor
        PistonGroup(const std::vector<pros::adi::Pneumatics> pistonsVec);

        // Piston Group class constructor
        PistonGroup(const std::vector<uint8_t> ports,
                    const bool startExtended,
                    const bool reversed = false);

        // Piston Group class constructor
        PistonGroup(const std::vector<pros::adi::ext_adi_port_pair_t> portPairs,
                    const bool startExtended,
                    const bool reversed = false);
        
        void extend();
        void retract();
        void toggle();
        std::vector<bool> are_extended();

    private:
        std::vector<pros::adi::Pneumatics> pistonsVec;
};

/**
 * @brief Construct a shared pointer to a pneumatic piston.
 *
 * This function exists to reduce complexity for the user. The user could make their own
 * shared pointer to a pneumatic piston, but this function makes it easy
 *
 * @param port the ADI port number (from 1-8, 'a'-'h', 'A'-'H') to configure
 * @param startExtended piston position on program start (true for extended, false for retracted).
 * @param reversed sets whether the the pneumatic is extended when the ADI receives high or low.
 *                 When true, the piston extends when the ADI input is false.
 * @return `std::shared_ptr<pros::adi::Pneumatics>` a shared pointer to the pneumatic piston.
 */
[[nodiscard("returns a shared_ptr to a pros::adi::Pneumatics")]] std::shared_ptr<pros::adi::Pneumatics>
makePiston(const std::uint8_t port, const bool startExtended, const bool reversed = false);

/**
 * @brief Construct a shared pointer to a group of pistons.
 *
 * This function exists to reduce complexity for the user. The user could make their own
 * shared pointer to a group of pistons, but this function makes it easy
 *
 * @param pistons the vector of `pros::adi::Pneumatics`
 * 
 * @return `std::shared_ptr<PistonGroup>` a shared pointer to the piston group.
 */
[[nodiscard("returns a shared_ptr to a lemlib::PistonGroup")]] std::shared_ptr<PistonGroup>
makePistonGroup(const std::vector<pros::adi::Pneumatics> pistonsVec);

/**
 * @brief Construct a shared pointer to a group of pistons.
 *
 * This function exists to reduce complexity for the user. The user could make their own
 * shared pointer to a group of pistons, but this function makes it easy
 *
 * @param port the ADI port number (from 1-8, 'a'-'h', 'A'-'H') to configure
 * @param startExtended piston position on program start (true for extended, false for retracted).
 * @param reversed sets whether the the pneumatic is extended when the ADI receives high or low.
 *                 When true, the piston extends when the ADI input is false.
 * @return `std::shared_ptr<std::vector<pros::adi::Pneumatics>>` a shared pointer to the piston group.
 */
[[nodiscard("returns a shared_ptr to a lemlib::PistonGroup")]] std::shared_ptr<PistonGroup>
makePistonGroup(const std::vector<uint8_t> ports, const bool startExtended, const bool reversed = false);

/**
 * @brief Construct a shared pointer to a group of pistons.
 *
 * This function exists to reduce complexity for the user. The user could make their own
 * shared pointer to a group of pistons, but this function makes it easy
 *
 * @param portPairs the ADI port number (from 1-8, 'a'-'h', 'A'-'H') to configure
 * @param startExtended piston position on program start (true for extended, false for retracted).
 * @param reversed sets whether the the pneumatic is extended when the ADI receives high or low.
 *                 When true, the piston extends when the ADI input is false.
 * @return `std::shared_ptr<std::vector<pros::adi::Pneumatics>>` a shared pointer to the piston group.
 */
[[nodiscard("returns a shared_ptr to a lemlib::PistonGroup")]] std::shared_ptr<PistonGroup>
makePistonGroup(const std::vector<pros::adi::ext_adi_port_pair_t> portPairs,
                const bool startExtended,
                const bool reversed = false);
} // namespace lemlib