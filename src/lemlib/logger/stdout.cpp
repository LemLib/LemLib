#include <iostream>
#include <ostream>

#include "lemlib/logger/stdout.hpp"

namespace lemlib {
Stdout::Stdout() : Buffer([](const std::string& text) { std::cout << text << std::flush; }) { setRate(50); }
} // namespace lemlib