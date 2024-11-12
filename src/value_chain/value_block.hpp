#ifndef VALUE_BLOCK_HPP
#define VALUE_BLOCK_HPP

#include "../common/types.hpp"
#include "../cryptography/cryptography.hpp"
#include "transaction.hpp"
#include <vector>

class ValueBlock {
public:
    // Constructor for creating a new block
    ValueBlock(const Hash& previous_hash,
               const Hash& time_block_hash,
               TimePoint time,
               const std::vector<Transaction>& transactions,
               const PublicKey& public_key);

    // Default constructor for deserialization
    ValueBlock();

    // Accessor methods
    const Hash& get_previous_hash() const;
    const Hash& get_time_block_hash() const;
    const TimePoint& get_time() const;
    const std::vector<Transaction>& get_transactions() const;
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
    Hash time_block_hash_; // Reference to the corresponding TimeBlock
    TimePoint time_;
    std::vector<Transaction> transactions_;
    PublicKey public_key_;
    Signature signature_;
    Hash hash_;
};

#endif // VALUE_BLOCK_HPP
