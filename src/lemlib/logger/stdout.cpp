#include "stdout.hpp"

#include <iostream>
#include <ostream>

namespace lemlib {
Stdout::Stdout() : Buffer([](const std::string& text) { std::cout << text << std::flush; }) { setRate(50); }
} // namespace lemlib