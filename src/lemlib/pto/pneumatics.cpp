#include "lemlib/pto/pneumatics.hpp"

namespace lemlib {
// #region PistonGroup member functions

// PistonGroup class constructors
PistonGroup::PistonGroup(const std::vector<pros::adi::Pneumatics> pistonsVec)
    : pistonsVec(pistonsVec) {}

PistonGroup::PistonGroup(const std::vector<uint8_t> ports, const bool startExtended, const bool reversed) {
    for (std::uint8_t port : ports) {
        pistonsVec.push_back(pros::adi::Pneumatics(port, startExtended, reversed));
    }
}

PistonGroup::PistonGroup(const std::vector<pros::adi::ext_adi_port_pair_t> portPairs,
                         const bool startExtended,
                         const bool reversed) {
    for (pros::adi::ext_adi_port_pair_t portPair : portPairs) {
        pistonsVec.push_back(pros::adi::Pneumatics(portPair, startExtended, reversed));
    }
}

// PistonGroup extend
void PistonGroup::extend() {
    for (pros::adi::Pneumatics piston : pistonsVec) {
        piston.extend();
    }
}

// PistonGroup retract
void PistonGroup::retract() {
    for (pros::adi::Pneumatics piston : pistonsVec) {
        piston.retract();
    }
}

// PistonGroup toggle
void PistonGroup::toggle() {
    for (pros::adi::Pneumatics piston : pistonsVec) {
        piston.toggle();
    }
}

// PistonGroup get states
std::vector<bool> PistonGroup::are_extended() {
    std::vector<bool> statusesVec = {};
    for (pros::adi::Pneumatics piston : pistonsVec) {
        statusesVec.push_back(piston.is_extended());
    }
    return statusesVec;
}



// #endregion PistonGroup member functions

// #region makePiston and makePistonGroup functions
std::shared_ptr<pros::adi::Pneumatics> makePiston(const std::uint8_t port,
                                                  const bool startExtended,
                                                  const bool reversed) {
    auto pneumaticsPtr = std::make_shared<pros::adi::Pneumatics>(port, startExtended, reversed);
    return pneumaticsPtr;
}

std::shared_ptr<PistonGroup> makePistonGroup(const std::vector<pros::adi::Pneumatics> pistonsVec) {
	std::shared_ptr<PistonGroup> pistonGroupPtr = std::make_shared<PistonGroup>(PistonGroup(pistonsVec));
	return pistonGroupPtr;
};

std::shared_ptr<PistonGroup> makePistonGroup(const std::vector<uint8_t> ports,
                                             const bool startExtended,
                                             const bool reversed) {
    PistonGroup pistonGroup(ports, startExtended, reversed);
    std::shared_ptr<PistonGroup> pistonGroupPtr = std::make_shared<PistonGroup>(pistonGroup);
	return pistonGroupPtr;
};

std::shared_ptr<PistonGroup> makePistonGroup(const std::vector<pros::adi::ext_adi_port_pair_t> portPairs,
                                             const bool startExtended,
                                             const bool reversed) {
    PistonGroup pistonGroup(portPairs, startExtended, reversed);
    std::shared_ptr<PistonGroup> pistonGroupPtr = std::make_shared<PistonGroup>(pistonGroup);
	return pistonGroupPtr;
};
// #endregion makePiston and makePistonGroup functions

} // namespace lemlib