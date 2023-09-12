#include "abstractSink.hpp"

void lemlib::AbstractSink::setLowestLevel(lemlib::AbstractSink::Level lowestLevel) { this->lowestLevel = lowestLevel; }

void lemlib::AbstractSink::setFormat(const std::string& logFormat) { this->logFormat = logFormat; }