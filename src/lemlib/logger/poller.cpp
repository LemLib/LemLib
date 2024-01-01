
#include "lemlib/logger/poller.hpp"

namespace lemlib {

std::vector<char> getSDCardData(FILE* filePointerArg, int bufferLengthArg) {

    std::vector<char> fileContent;

    fileContent.resize(bufferLengthArg);

    // Saves the file content to the vector
    fread(&fileContent, sizeof(fileContent)[0], (fileContent.size()) / sizeof(fileContent[0]), filePointerArg);

    return fileContent;

}

}

