
#include "lemlib/eventhandler/event.hpp"
#include "pros/apix.h"
#include "lemlib/eventhandler/eventhandler.hpp"
#include <exception>
#include <unordered_map>

namespace lemlib {

EventHandler::EventHandler() {
    eventVector.clear();
    eventStates.clear();
}

EventHandler::~EventHandler() {
    eventVector.clear();
    eventStates.clear();
}

EventHandler::EventHandler(std::vector<std::shared_ptr<Event>>& eventVector) {
    this->eventVector = eventVector;
    eventStates.resize(eventVector.size());

    // Bubble Sort the Event IDs

    for (int i = 0; i < eventVector.size(); i++) {
        for (int j = 0; j < eventVector.size() - i - 1; j++) {
            if (eventVector.at(j)->getId() > eventVector.at(j + 1)->getId()) {
                std::swap(eventVector.at(j), eventVector.at(j + 1));
            }
        }
    }

    std::cout << "    Event Vector:  ";
    for (auto& event : eventVector) { std::cout << " " << event->getId() << ", "; }
    std::cout << std::endl;
    for (int i = 0; i < eventVector.size() - 1; i++) {
        // If the ID is the same as the next ID, then throw an exception.
        if (eventVector.at(i)->getId() != eventVector.at(i + 1)->getId()) {
            std::cout << "Valid IDs: " << eventVector.at(i)->getId() << " != " << eventVector.at(i + 1)->getId()
                      << std::endl;
        } else {
            std::cout << "Event IDs at " << i << ", " << i + 1 << " of " << eventVector.at(i)->getId() << " = "
                      << eventVector.at(i + 1)->getId() << " are not unique. ";
            std::cout << "Throwing exception." << std::endl;
            throw std::exception();
            pros::delay(200);
        }
    }
}

std::vector<bool>& EventHandler::getCurrentEvents(uint id) { return eventStates; }

bool EventHandler::checkEvent(uint id) { 

    bool eventResult = false;

    for (auto event : eventVector) {
        if (id == event->getId()) {
            eventResult = event->check();
        }
        
    }

    return eventResult; 
}

void EventHandler::startAsyncTask() {
    pros::Task task {[this] {
        while (true) {
            
            for (int i = 0; i < eventVector.size(); i++) {
                if (eventVector.at(i).get()->check()) {
                    eventStates.at(i) = true;
                } else {
                    eventStates.at(i) = false;
                }
                
            }
            pros::delay(10);
            
        }
    }};
}

std::vector<uint> EventHandler::getAllIDs() {

    std::vector<uint> ids;

    for (auto event : eventVector) {
        ids.emplace_back(event->getId());
    }

    return ids;
}

bool EventHandler::isValidID(uint id) {
    for (auto& event : eventVector) {
        if (event->getId() == id) { return true; }
    }
    return false;
}

} // namespace lemlib