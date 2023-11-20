#pragma once

#include "path.hpp"

namespace lemlib {
class FilePath : public Path {
    public:
        /**
         * @brief Construct a new File Path
         *
         * @param path the path to load the path from
         */
        FilePath(const char* filePath);
};
} // namespace lemlib