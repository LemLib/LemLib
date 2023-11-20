#include "lemlib/path/assetPath.hpp"
#include "util.hpp"

lemlib::AssetPath::AssetPath(const asset& asset) {
    this->path = {};

    // parse the asset
    std::string input(reinterpret_cast<char*>(asset.buf), asset.size);
    std::vector<std::string> lines = splitString(input, "\n");

    this->load(lines);
}