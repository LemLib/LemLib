/**
 * @file include/lemlib/pto.hpp
 * @author LemLib Team
 * @brief PTO class declarations
 * @version 0.4.7
 * @date 2023-12-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <memory>

#include "pros/adi.hpp"
#include "pros/motor_group.hpp"

namespace lemlib {
/**
 * @brief Construct a shared pointer to a pneumatic piston.
 *
 * This function exists to reduce complexity for the client. The client could make their own
 * shared pointer to a pneumatic piston, but this function makes it easy
 *
 * @param port the ADI port number (from 1-8, 'a'-'h', 'A'-'H') to configure
 * @param startExtended piston position on program start (true for extended, false for retracted).
 * @param reversed sets whether the the pneumatic is extended when the ADI receives high or low.
 *                 When true, the piston extends when the ADI input is false.
 * @return `std::shared_ptr<pros::adi::Pneumatics>` a shared pointer to the pneumatic piston.
 */
[[nodiscard("makePiston function returns a shared_ptr of pros::adi::Pneumatics, you can use "
            "std::shared_ptr<pros::adi::Pneumatics> or auto to get the returned pointer")]] std::shared_ptr<pros::adi::Pneumatics>
makePiston(const std::uint8_t port, const bool startExtended, const bool reversed = false);

class PTO {
    public:
        /**
         * @brief 
         */
        PTO(std::vector<std::shared_ptr<pros::adi::Pneumatics>> pistons, std::shared_ptr<pros::MotorGroup> motors);
    private:
        std::vector<pros::adi::Pneumatics*> pistons;
};
} // namespace lemlib