#include <algorithm>
#include <memory>
#include <vector>
#include <functional>

namespace lemlib {

class Event {
    protected:
    public:
        virtual bool check() = 0;
};

} // namespace lemlib