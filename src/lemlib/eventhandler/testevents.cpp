
#include "lemlib/eventhandler/testevents.hpp"

namespace lemlib {

TESTEvent::TESTEvent(bool state, uint id)  {
    this->state = state;
    this->id = id;
}

bool TESTEvent::check() {
    return state;
}

} // namespace lemlib