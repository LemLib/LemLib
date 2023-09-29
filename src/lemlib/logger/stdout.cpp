#include "stdout.hpp"

namespace lemlib {
Stdout::Stdout() : Buffer([](const std::string& text) { fmt::print(text); }) { setRate(50); }
} // namespace lemlib