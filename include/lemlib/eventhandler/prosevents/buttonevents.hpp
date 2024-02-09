
#include "pros/device.hpp"
#include "pros/motors.hpp"
#include "eventhandler/event.hpp"
#include <memory>

namespace lemlib {
class PROSButtonEvent : public Event {
    protected:
        pros::controller_digital_e_t button;
        std::shared_ptr<pros::Controller> controller;
    public:
        PROSButtonEvent(std::shared_ptr<pros::Controller> controller, pros::controller_digital_e_t button);

        bool check() override;
};
} // namespace lemlib