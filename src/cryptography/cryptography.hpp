#ifndef CRYPTOGRAPHY_HPP
#define CRYPTOGRAPHY_HPP

#include "types.hpp"
#include <string>

namespace cryptography {

// Hashing functions
Hash sha256(const bytes& data);
Hash double_sha256(const bytes& data);

// Key generation
PrivateKey generate_private_key();
PublicKey derive_public_key(const PrivateKey& private_key);

// Address generation
std::string public_key_to_address(const PublicKey& public_key);

// Other cryptographic utilities
bool verify_signature(const bytes& message, const Signature& signature, const PublicKey& public_key);
Signature sign_message(const bytes& message, const PrivateKey& private_key);

} // namespace cryptography

#endif // CRYPTOGRAPHY_HPP
