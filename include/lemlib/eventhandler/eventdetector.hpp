
#include <functional>
#include <algorithm>
#include <memory>

#include "lemlib/logger/stdout.hpp"
#include "lemlib/eventhandler/event.hpp"
#include "lemlib/logger/logger.hpp"

namespace lemlib {

enum class E_EVENT_TYPE { BUTTON = 0, JOYSTICK = 1 };

class EventDetector {
    protected:
        std::unique_ptr<lemlib::BufferedStdout> logger;
        std::vector<std::unique_ptr<Event>> buttonEvents;
    public:

        EventDetector();
        EventDetector(std::unique_ptr<lemlib::BufferedStdout> logger) : logger(std::move(logger)) {};
        ~EventDetector();

        /**
         * @brief Checks to see if the function is true, if it is, log the type of event it is.
         * 
         * @tparam T 
         * @param type 
         * @param func 
         * @return true 
         * @return false 
         */
        template <class T> void addEvent(E_EVENT_TYPE type, std::function<bool(int)> func);

        /**
         * @brief Start an Asynchronous task that loops through all the events and checks them every X milliseconds.
         * 
         * @param msDelay 
         */
        virtual void autoLoop(int msDelay = 10) = 0; 

        /**
         * @brief Get a reference to an Event at index X.
         * 
         * @param index 
         * @return std::unique_ptr<Event>& 
         */
        std::unique_ptr<Event>& getEvent(int index);

};

} // namespace lemlib