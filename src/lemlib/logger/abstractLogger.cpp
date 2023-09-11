#include "abstractLogger.hpp"

void lemlib::AbstractLogger::setLowestLevel(lemlib::AbstractLogger::Level lowestLevel) {
    this->lowestLevel = lowestLevel;
}

void lemlib::AbstractLogger::setFormat(const std::string& logFormat) { this->logFormat = logFormat; }