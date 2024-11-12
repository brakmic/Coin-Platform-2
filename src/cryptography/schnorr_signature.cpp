#include "schnorr_signature.hpp"
#include "cryptography.hpp"
#include <secp256k1.h>
#include <secp256k1_schnorrsig.h>
#include <stdexcept>

Signature SchnorrSignature::sign(const bytes &message, const PrivateKey &private_key)
{
    Signature signature;
    Hash hash = cryptography::sha256(message);

    secp256k1_keypair keypair;
    if (!secp256k1_keypair_create(get_context(), &keypair, private_key.data()))
    {
        throw std::runtime_error("Failed to create keypair for signing");
    }

    if (!secp256k1_schnorrsig_sign(get_context(), signature.data(), hash.data(), &keypair, nullptr))
    {
        throw std::runtime_error("Failed to sign message");
    }

    return signature;
}

PublicKey SchnorrSignature::derive_public_key(const PrivateKey &private_key)
{
    PublicKey public_key;

    secp256k1_keypair keypair;
    if (!secp256k1_keypair_create(get_context(), &keypair, private_key.data()))
    {
        throw std::runtime_error("Failed to create keypair for public key derivation");
    }

    secp256k1_xonly_pubkey xonly_pubkey;
    if (!secp256k1_keypair_xonly_pub(get_context(), &xonly_pubkey, nullptr, &keypair))
    {
        throw std::runtime_error("Failed to extract x-only public key");
    }

    if (!secp256k1_xonly_pubkey_serialize(get_context(), public_key.data(), &xonly_pubkey))
    {
        throw std::runtime_error("Failed to serialize public key");
    }

    return public_key;
}

bool SchnorrSignature::verify(const bytes &message, const Signature &signature, const PublicKey &public_key)
{
    Hash hash = cryptography::sha256(message);

    secp256k1_xonly_pubkey pubkey;
    if (!secp256k1_xonly_pubkey_parse(get_context(), &pubkey, public_key.data()))
    {
        return false;
    }

    int result = secp256k1_schnorrsig_verify(get_context(), signature.data(), hash.data(), 32, &pubkey);
    return result == 1;
}

secp256k1_context *SchnorrSignature::get_context()
{
    static secp256k1_context *context = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    return context;
}
