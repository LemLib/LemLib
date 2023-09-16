#include "typeFormatting.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

namespace lemlib {
std::string format_as(const Pose& pose) {
    // the double brackets become single brackets
    return fmt::format("lemlib::Pose {{ x: {}, y: {}, theta: {} }}", pose.x, pose.y, pose.theta);
}
} // namespace lemlib