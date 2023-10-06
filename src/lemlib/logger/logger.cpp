#include "logger.hpp"

namespace lemlib {

Logger& Logger::getLogger() {
    static Logger logger;
    return logger;
}

} // namespace lemlib
