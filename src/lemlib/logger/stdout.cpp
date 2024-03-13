#include <iostream>
#include <ostream>

#include "lemlib/logger/stdout.hpp"

namespace lemlib {
BufferedStdout::BufferedStdout()
    : Buffer([](const std::string& text) { std::cout << text << std::flush; }) {
    setRate(50);
}

BufferedStdout& bufferedStdout() {
    static BufferedStdout bufferedStdout;
    return bufferedStdout;
}
} // namespace lemlib