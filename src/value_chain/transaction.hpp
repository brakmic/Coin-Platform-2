#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include "../common/types.hpp"
#include "../cryptography/cryptography.hpp"
#include <vector>

class Transaction
{
public:
  // Constructor for coinbase transactions
  Transaction(const PublicKey &recipient_public_key, uint64_t amount);
  // Constructor for creating a new transaction
  Transaction(const PublicKey &sender_public_key,
              const PublicKey &recipient_public_key,
              uint64_t amount,
              const std::vector<byte> &data = {});

  // Default constructor for deserialization
  Transaction();

  // Accessor methods
  const PublicKey &get_sender_public_key() const;
  const PublicKey &get_recipient_public_key() const;
  uint64_t get_amount() const;
  const std::vector<byte> &get_data() const;
  const Signature &get_signature() const;
  const Hash &get_hash() const;

  // Sets the signature after signing
  void set_signature(const Signature &signature);

  // Serialization and deserialization
  bytes serialize() const;
  bool deserialize(const bytes &data);

  // Gets the data to be signed
  bytes get_data_to_sign() const;

  // Computes the transaction's hash
  void compute_hash();

  // Verifies the transaction's signature
  bool verify() const;

  bool is_coinbase_transaction() const;

  bool operator==(const Transaction &other) const;

private:
  PublicKey sender_public_key_;
  PublicKey recipient_public_key_;
  uint64_t amount_;
  std::vector<byte> data_; // Optional data payload
  Signature signature_;
  Hash hash_;
};

#endif // TRANSACTION_HPP
