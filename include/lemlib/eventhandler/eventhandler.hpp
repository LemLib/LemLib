
#include "lemlib/logger/logger.hpp"
#include "lemlib/eventhandler/event.hpp"

namespace lemlib {

template <class T> class EventHandler {
    protected:
        std::vector<std::shared_ptr<Event<T>>> eventVector;
        std::vector<bool> eventStates;
    public:
        EventHandler(std::vector<std::shared_ptr<Event<T>>> eventVector);
        ~EventHandler();

        std::vector<bool>& getCurrentEvents();

        void startAsyncTask();
};

} // namespace lemlib