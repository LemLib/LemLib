#pragma once

#include "lemlog/logger/logger.hpp"

namespace logger {
/**
 * @brief SD card class. Outputs all data to the SD card connected to the brain.
 */
class SDCard : public Sink {
    public:
        SDCard(std::string filename = ".log", bool logTimestamp = true);
        void send(Level level, std::string topic, std::string message) override;
    private:
        std::string formatTimestamp(long long ms);

        std::string filename;
        bool logTimestamp;
};
}