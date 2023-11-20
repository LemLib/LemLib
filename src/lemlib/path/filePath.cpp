#include "lemlib/path/filePath.hpp"
#include <fstream>

std::vector<std::string> readLines(const char* filePath) {
    std::string path = "/usd/" + std::string(filePath);
    std::vector<std::string> lines = {};
    std::ifstream file(path, std::ios::in);

    std::string line;
    while (std::getline(file, line)) { lines.push_back(line); }

    file.close();
    return lines;
}

lemlib::FilePath::FilePath(const char* filePath) {
    this->path = {};

    // parse the file
    std::vector<std::string> lines = readLines(filePath);

    this->load(lines);
}