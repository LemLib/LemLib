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

    public:
        virtual bool check() = 0;

        uint getId() { return id; }
};

} // namespace lemlib