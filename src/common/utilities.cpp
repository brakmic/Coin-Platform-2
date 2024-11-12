#include "utilities.hpp"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace utilities
{

  bytes hex_to_bytes(const std::string &hex)
  {
    bytes result;
    if (hex.length() % 2 != 0)
    {
      log_error("Invalid hex string length.");
      return result;
    }

    for (size_t i = 0; i < hex.length(); i += 2)
    {
      std::string byte_string = hex.substr(i, 2);
      byte byte_value = static_cast<byte>(std::stoul(byte_string, nullptr, 16));
      result.push_back(byte_value);
    }

    return result;
  }

  std::string bytes_to_hex(const bytes &data)
  {
    std::stringstream ss;
    for (byte b : data)
    {
      ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b);
    }
    return ss.str();
  }

  TimePoint get_current_time()
  {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
  }

  void log_info(const std::string &message)
  {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::cout << "[INFO] [" << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") << "] " << message << std::endl;
  }

  void log_error(const std::string &message)
  {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::cerr << "[ERROR] [" << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") << "] " << message << std::endl;
  }

} // namespace utilities
