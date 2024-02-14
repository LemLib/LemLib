
#include "pros/apix.h"
#include "lemlib/eventhandler/eventhandler.hpp"
#include <exception>
#include <unordered_map>

namespace lemlib {

EventHandler::~EventHandler() {
    eventVector.clear();
    eventStates.clear();
}

EventHandler::EventHandler(std::vector<std::shared_ptr<Event>> eventVector) { 
    this->eventVector = eventVector; 

    // Bubble Sort the Event IDs

    for (int i = 0; i < eventVector.size(); i++) {
        for (int j = 0; j < eventVector.size() - i - 1; j++) {
            if (eventVector.at(j)->getId() > eventVector.at(j + 1)->getId()) {
                std::swap(eventVector.at(j), eventVector.at(j + 1));
            }
        }
    }

    for (int i = 0; i < eventVector.size() - 1; i++) {
        // If the ID is the same as the next ID, then throw an exception.
        if (eventVector.at(i)->getId() == eventVector.at(i+1)->getId()) {
            continue;
        } else {
            std::cout << "Event IDs at " << i << ", " << i + 1 << " are not unique. Throwing exception.";
            throw std::exception();
        }
    }

}

std::vector<bool>& EventHandler::getCurrentEvents(uint id) { return eventStates; }

bool EventHandler::checkEvent(uint id) { return eventStates.at(id); }

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