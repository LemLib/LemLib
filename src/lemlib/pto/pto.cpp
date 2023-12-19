#include "lemlib/pto/pto.hpp"

namespace lemlib {
std::shared_ptr<pros::adi::Pneumatics> makePiston(const std::uint8_t port,
                                                  const bool startExtended,
                                                  const bool reversed) {
    auto pneumaticsPtr = std::make_shared<pros::adi::Pneumatics>(port, startExtended, reversed);
    return pneumaticsPtr;
}
} // namespace lemlib
