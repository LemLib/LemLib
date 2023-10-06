#include "message.hpp"

namespace lemlib {
Message::Message(Level level, Type type, std::string message) : level(level), type(type), message(message) {};

} // namespace lemlib
