#ifndef TIME_BLOCK_HPP
#define TIME_BLOCK_HPP

#include "../common/types.hpp"
#include "../cryptography/cryptography.hpp"

class TimeBlock {
public:
    // Constructor for creating a new block
    TimeBlock(const Hash& previous_hash, TimePoint time, const PublicKey& public_key);

    // Default constructor for deserialization
    TimeBlock();

    // Accessor methods
    const Hash& get_previous_hash() const;
    const TimePoint& get_time() const;
    const PublicKey& get_public_key() const;
    const Signature& get_signature() const;
    const Hash& get_hash() const;

    // Sets the signature after signing
    void set_signature(const Signature& signature);

    // Serialization and deserialization
    bytes serialize() const;
    bool deserialize(const bytes& data);

    // Gets the data to be signed
    bytes get_data_to_sign() const;

    // Computes the block's hash
    void compute_hash();

private:
    Hash previous_hash_;
    TimePoint time_;
    PublicKey public_key_;
    Signature signature_;
    Hash hash_;
};

#endif // TIME_BLOCK_HPP
