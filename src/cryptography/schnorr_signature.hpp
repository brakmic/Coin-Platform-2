#ifndef SCHNORR_SIGNATURE_HPP
#define SCHNORR_SIGNATURE_HPP

#include "types.hpp"
#include <secp256k1.h>
#include <secp256k1_schnorrsig.h>

class SchnorrSignature
{
public:
    // Derives the public key from the private key
    static PublicKey derive_public_key(const PrivateKey &private_key);

    // Signs a message using the private key
    static Signature sign(const bytes &message, const PrivateKey &private_key);

    // Verifies a signature given the message and public key
    static bool verify(const bytes &message, const Signature &signature, const PublicKey &public_key);

private:
    static secp256k1_context *get_context();
};

#endif // SCHNORR_SIGNATURE_HPP
