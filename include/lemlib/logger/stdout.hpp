#pragma once
#include "buffer.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

namespace lemlib {
/**
 * @brief Buffered printing to Stout.
 *
 * LemLib uses a buffered wrapper around stdout in order to guarantee that messages are printed at a constant
 * rate, no matter how many different threads are trying to use the logger. This is a concern  because not every type of
 * connection to the brain has the same amount of bandwidth.
 */
class Stdout : protected Buffer {
    public:
        Stdout(const Stdout&) = delete;
        Stdout operator=(const Stdout&) = delete;

        /**
         * @brief Print a string (thread-safe).
         *
         */
        template <typename... T> static void print(fmt::format_string<T...> format, T&&... args) {
            static Stdout theStdout;
            theStdout.pushToBuffer(fmt::format(format, std::forward<T>(args)...));
        }
    private:
        Stdout();
        ~Stdout() = default;
};
} // namespace lemlib
