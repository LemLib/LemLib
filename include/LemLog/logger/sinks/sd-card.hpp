#pragma once

#include "lemlog/logger/Sink.hpp"

namespace logger {
/**
 * @brief SD card class. Outputs all data to the SD card connected to the brain.
 */
class SDCard : public Sink {
    public:
        SDCard(std::string filename = ".log", bool logTimestamp = true);
        SinkStatus write(Level level, const std::string& topic,
                         const std::string& message) override;
    private:
        std::string formatTimestamp(long long ms);

        std::string filename;
        bool logTimestamp;
};
} // namespace logger