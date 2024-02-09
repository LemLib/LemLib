
#include "lemlib/logger/logger.hpp"
#include "lemlib/eventhandler/event.hpp"

namespace lemlib {

class EventHandler {
    protected:
        std::vector<std::shared_ptr<Event>> eventVector;
        std::vector<bool> eventStates;
    public:
        EventHandler(std::vector<std::shared_ptr<Event>> eventVector);
        ~EventHandler();

        std::vector<bool>& getCurrentEvents();

        void startAsyncTask();
};

} // namespace lemlib