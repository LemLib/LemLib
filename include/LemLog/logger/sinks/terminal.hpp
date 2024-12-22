#pragma once

#include "lemlog/logger/logger.hpp"

namespace logger {
/**
 * @brief Terminal class. Outputs to stdout (the PROS terminal)
 *
 */
class Terminal : public Sink {
    public:
        Terminal(bool disableCOBS = false, bool disableStreamID = false);
        void send(Level level, std::string topic, std::string message) override;
};
} // namespace logger