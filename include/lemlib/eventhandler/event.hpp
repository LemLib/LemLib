#include <algorithm>
#include <memory>
#include <vector>
#include <functional>

namespace lemlib {

template <class T> class Event {
    protected:
        std::shared_ptr<std::function<T>> function = nullptr;
    public:
        Event(std::shared_ptr<std::function<T>> function)
            : function(function) {};

        virtual bool check() = 0;
};

} // namespace lemlib