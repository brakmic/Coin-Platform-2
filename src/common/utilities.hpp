#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <string>
#include "types.hpp"

namespace utilities {

// Converts a hexadecimal string to a byte vector
bytes hex_to_bytes(const std::string& hex);

// Converts a byte vector to a hexadecimal string
std::string bytes_to_hex(const bytes& data);

// Retrieves the current time as TimePoint
TimePoint get_current_time();

// Logging functions
void log_info(const std::string& message);
void log_error(const std::string& message);

} // namespace utilities

#endif // UTILITIES_HPP
