
#include "pros/apix.h"
#include "lemlib/eventhandler/eventhandler.hpp"

namespace lemlib {

EventHandler::~EventHandler() {
    eventVector.clear();
    eventStates.clear();
}

EventHandler::EventHandler(std::vector<std::shared_ptr<Event>> eventVector) { this->eventVector = eventVector; }

std::vector<bool>& EventHandler::getCurrentEvents() { return eventStates; }

void EventHandler::startAsyncTask() {
    pros::Task task {[this] {
        while (true) {
            for (int i = 0; i > eventVector.size(); i++) {
                if (eventVector.at(i).get()->check()) { eventStates.at(i) = true; }
            }
            pros::delay(10);
        }
    }};
}

} // namespace lemlib