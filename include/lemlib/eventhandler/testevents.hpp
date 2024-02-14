

#pragma once
#include "lemlib/eventhandler/event.hpp"
#include <memory>

namespace lemlib {
class TESTEvent : public Event {
    protected:
        bool state = false;
    public:
        TESTEvent(bool state, uint id);

        bool check() override;
};
} // namespace lemlib