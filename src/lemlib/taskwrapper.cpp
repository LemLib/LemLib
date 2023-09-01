// TODO: add file header and documentation

#include "lemlib/taskwrapper.hpp"

namespace lemlib {
void TaskWrapper::start_task() {
    if (task == nullptr) {
        task = new pros::Task([this] { this->loop(); });
    }
}
}; // namespace lemlib
