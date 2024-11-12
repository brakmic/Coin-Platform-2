#include "cryptography.hpp"
#include "schnorr_signature.hpp"
#include "utilities.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>

namespace cryptography
{

  Hash sha256(const bytes &data)
  {
    Hash hash;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_sha256();

    EVP_DigestInit_ex(ctx, md, nullptr);
    EVP_DigestUpdate(ctx, data.data(), data.size());
    EVP_DigestFinal_ex(ctx, hash.data(), nullptr);

    EVP_MD_CTX_free(ctx);
    return hash;
  }

  Hash double_sha256(const bytes &data)
  {
    Hash first_hash = sha256(data);
    return sha256(bytes(first_hash.begin(), first_hash.end()));
  }

  PrivateKey generate_private_key()
  {
    PrivateKey private_key;
    RAND_bytes(private_key.data(), private_key.size());
    return private_key;
  }

  PublicKey derive_public_key(const PrivateKey &private_key)
  {
    return SchnorrSignature::derive_public_key(private_key);
  }

  std::string public_key_to_address(const PublicKey &public_key)
  {
    // Perform SHA256 hash
    Hash sha_hash = sha256(bytes(public_key.begin(), public_key.end()));

    // Perform RIPEMD160 hash using EVP
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_ripemd160();

    unsigned char ripemd_hash[EVP_MAX_MD_SIZE];
    unsigned int ripemd_length = 0;

    EVP_DigestInit_ex(ctx, md, nullptr);
    EVP_DigestUpdate(ctx, sha_hash.data(), sha_hash.size());
    EVP_DigestFinal_ex(ctx, ripemd_hash, &ripemd_length);

    EVP_MD_CTX_free(ctx);

    // Convert to bytes
    bytes ripemd_bytes(ripemd_hash, ripemd_hash + ripemd_length);

    // For simplicity, encode the RIPEMD160 hash in hexadecimal
    std::string address = utilities::bytes_to_hex(ripemd_bytes);
    return address;
  }

  Signature sign_message(const bytes &message, const PrivateKey &private_key)
  {
    return SchnorrSignature::sign(message, private_key);
  }

  bool verify_signature(const bytes &message, const Signature &signature, const PublicKey &public_key)
  {
    return SchnorrSignature::verify(message, signature, public_key);
  }

} // namespace cryptography
