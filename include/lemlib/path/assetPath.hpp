#pragma once

#include "lemlib/asset.hpp"
#include "path.hpp"

namespace lemlib {
class AssetPath : public Path {
    public:
        /**
         * @brief Construct a new Asset Path
         *
         * @param asset the asset to load the path from
         */
        AssetPath(const asset& asset);
};
} // namespace lemlib