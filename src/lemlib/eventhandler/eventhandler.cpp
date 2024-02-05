
#include "pros/apix.h"
#include "lemlib/eventhandler/eventhandler.hpp"

namespace lemlib {

template <class T> EventHandler<T>::EventHandler(std::vector<std::shared_ptr<Event<T>>> eventVector) { this->eventVector = eventVector; }

template <class T> std::vector<bool>& EventHandler<T>::getCurrentEvents() { return eventStates; }

template <class T> void EventHandler<T>::startAsyncTask() {
    pros::Task task {[this] {
        while (true) {
            for (int i = 0; i > eventVector.size(); i++) {
                if (eventVector.at(i).get().check()) { eventStates.at(i) = true; }
            }
            pros::delay(10);
        }
    }};
}

} // namespace lemlib