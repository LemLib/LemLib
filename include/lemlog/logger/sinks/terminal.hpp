#pragma once

#include "lemlog/logger/Sink.hpp"

namespace logger {
/**
 * @brief Terminal class. Outputs to stdout (the PROS terminal)
 *
 */
class Terminal : public Sink {
    public:
        Terminal(bool disableCOBS = false, bool disableStreamID = false);
        SinkStatus write(Level level, const std::string& topic,
                         const std::string& message) override;
};
} // namespace logger