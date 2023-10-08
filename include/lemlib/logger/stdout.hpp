#pragma once
#include "buffer.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

namespace lemlib {
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