#pragma once

#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <vector>
#include <functional>

namespace lemlib {

class Event {
    protected:
        uint id;

        Event() {
            try {
                if (id > -1) {
                    std::cout << "Proper identification.";
                } 
                else {
                    std::cout << "Negative ID Value not allowed. Throwing exception. ";
                    throw std::exception();
                }
            }
            catch (std::exception& e) {
                std::cout << "Improper identification. Add ID Parameter to the Event.";
            
            }
        }
    public:
        virtual bool check() = 0;

        uint getId() { return id; }
};

} // namespace lemlib