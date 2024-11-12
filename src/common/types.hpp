#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>
#include <array>
#include <vector>
#include <string>

// Define fixed-size types for consistency
using byte = uint8_t;
using bytes = std::vector<byte>;

// Hash types
constexpr size_t HASH_SIZE = 32;
using Hash = std::array<byte, HASH_SIZE>;

// Public and private key types
constexpr size_t PUBLIC_KEY_SIZE = 32;
using PublicKey = std::array<byte, PUBLIC_KEY_SIZE>;

constexpr size_t PRIVATE_KEY_SIZE = 32;
using PrivateKey = std::array<byte, PRIVATE_KEY_SIZE>;

// Signature type
constexpr size_t SIGNATURE_SIZE = 64;
using Signature = std::array<byte, SIGNATURE_SIZE>;

// Time representation (nanoseconds since epoch)
using TimePoint = uint64_t;

// Genesis Time (nanoseconds since epoch)
constexpr TimePoint GENESIS_TIME = 1730467980ULL * 1000000000ULL;

// Network-related types
using Port = uint16_t;
using IPAddress = std::string;

#endif // TYPES_HPP
