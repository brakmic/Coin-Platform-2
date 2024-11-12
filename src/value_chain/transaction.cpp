#include "transaction.hpp"
#include "../cryptography/cryptography.hpp"
#include "../common/utilities.hpp"
#include <cstring>
#include <algorithm>
#include <cassert>

Transaction::Transaction(const PublicKey &recipient_public_key, uint64_t amount)
    : sender_public_key_(), // Zero-initialized or special value
      recipient_public_key_(recipient_public_key),
      amount_(amount)
{
  signature_.fill(0);
  compute_hash();
}

Transaction::Transaction(const PublicKey &sender_public_key,
                         const PublicKey &recipient_public_key,
                         uint64_t amount,
                         const std::vector<byte> &data)
    : sender_public_key_(sender_public_key),
      recipient_public_key_(recipient_public_key),
      amount_(amount),
      data_(data)
{
  signature_.fill(0);
  compute_hash();
}

Transaction::Transaction()
    : amount_(0)
{
  sender_public_key_.fill(0);
  recipient_public_key_.fill(0);
  signature_.fill(0);
  hash_.fill(0);
}

const PublicKey &Transaction::get_sender_public_key() const
{
  return sender_public_key_;
}

const PublicKey &Transaction::get_recipient_public_key() const
{
  return recipient_public_key_;
}

uint64_t Transaction::get_amount() const
{
  return amount_;
}

const std::vector<byte> &Transaction::get_data() const
{
  return data_;
}

const Signature &Transaction::get_signature() const
{
  return signature_;
}

const Hash &Transaction::get_hash() const
{
  return hash_;
}

void Transaction::set_signature(const Signature &signature)
{
  signature_ = signature;
  compute_hash();
}

bytes Transaction::serialize() const
{
  bytes data;
  size_t expected_size = 32 + 32 + 8 + 8 + data_.size() + 64 + 32;
  data.reserve(expected_size);

  // Serialize sender public key (32 bytes)
  data.insert(data.end(), sender_public_key_.begin(), sender_public_key_.end());
  assert(data.size() == 32);

  // Serialize recipient public key (32 bytes)
  data.insert(data.end(), recipient_public_key_.begin(), recipient_public_key_.end());
  assert(data.size() == 64);

  // Serialize amount (8 bytes)
  bytes amount_bytes(reinterpret_cast<const byte *>(&amount_), reinterpret_cast<const byte *>(&amount_) + sizeof(uint64_t));
  data.insert(data.end(), amount_bytes.begin(), amount_bytes.end());
  assert(data.size() == 72);

  // Serialize data size (8 bytes)
  uint64_t data_size = data_.size();
  bytes data_size_bytes(reinterpret_cast<const byte *>(&data_size), reinterpret_cast<const byte *>(&data_size) + sizeof(uint64_t));
  data.insert(data.end(), data_size_bytes.begin(), data_size_bytes.end());
  assert(data.size() == 80);

  // Serialize data (variable length)
  data.insert(data.end(), data_.begin(), data_.end());
  assert(data.size() == 80 + data_.size());

  // Serialize signature (64 bytes)
  data.insert(data.end(), signature_.begin(), signature_.end());
  assert(data.size() == 80 + data_.size() + 64);

  // Serialize hash (32 bytes)
  data.insert(data.end(), hash_.begin(), hash_.end());
  assert(data.size() == expected_size);

  // Log detailed information
  utilities::log_info("Serialized Transaction fields:");
  utilities::log_info("Sender Public Key: " + utilities::bytes_to_hex(std::vector<byte>(sender_public_key_.begin(), sender_public_key_.end())));
  utilities::log_info("Recipient Public Key: " + utilities::bytes_to_hex(std::vector<byte>(recipient_public_key_.begin(), recipient_public_key_.end())));
  utilities::log_info("Amount: " + std::to_string(amount_));
  utilities::log_info("Data Size: " + std::to_string(data_size));
  utilities::log_info("Data: " + utilities::bytes_to_hex(data_));
  utilities::log_info("Signature: " + utilities::bytes_to_hex(std::vector<byte>(signature_.begin(), signature_.end())));
  utilities::log_info("Hash: " + utilities::bytes_to_hex(std::vector<byte>(hash_.begin(), hash_.end())));
  utilities::log_info("Total Serialized Transaction Size: " + std::to_string(data.size()) + " bytes.");

  return data;
}

bool Transaction::deserialize(const bytes &data)
{
  size_t offset = 0;

  // Deserialize sender public key (32 bytes)
  if (offset + sender_public_key_.size() > data.size())
  {
    utilities::log_error("Insufficient data for sender_public_key_. Expected " + std::to_string(sender_public_key_.size()) + ", got " + std::to_string(data.size() - offset));
    return false;
  }
  std::copy(data.begin() + offset, data.begin() + offset + sender_public_key_.size(), sender_public_key_.begin());
  offset += sender_public_key_.size();

  // Deserialize recipient public key (32 bytes)
  if (offset + recipient_public_key_.size() > data.size())
  {
    utilities::log_error("Insufficient data for recipient_public_key_. Expected " + std::to_string(recipient_public_key_.size()) + ", got " + std::to_string(data.size() - offset));
    return false;
  }
  std::copy(data.begin() + offset, data.begin() + offset + recipient_public_key_.size(), recipient_public_key_.begin());
  offset += recipient_public_key_.size();

  // Deserialize amount (8 bytes)
  if (offset + sizeof(uint64_t) > data.size())
  {
    utilities::log_error("Insufficient data for amount_. Expected " + std::to_string(sizeof(uint64_t)) + ", got " + std::to_string(data.size() - offset));
    return false;
  }
  std::memcpy(&amount_, &data[offset], sizeof(uint64_t));
  offset += sizeof(uint64_t);

  // Deserialize data size (8 bytes)
  uint64_t data_size = 0;
  if (offset + sizeof(uint64_t) > data.size())
  {
    utilities::log_error("Insufficient data for data_size. Expected " + std::to_string(sizeof(uint64_t)) + ", got " + std::to_string(data.size() - offset));
    return false;
  }
  std::memcpy(&data_size, &data[offset], sizeof(uint64_t));
  offset += sizeof(uint64_t);

  // Deserialize data (variable)
  if (offset + data_size > data.size())
  {
    utilities::log_error("Insufficient data for data_. Expected " + std::to_string(data_size) + ", got " + std::to_string(data.size() - offset));
    return false;
  }
  data_.resize(data_size);
  std::copy(data.begin() + offset, data.begin() + offset + data_size, data_.begin());
  offset += data_size;

  // Deserialize signature (64 bytes)
  if (offset + signature_.size() > data.size())
  {
    utilities::log_error("Insufficient data for signature_. Expected " + std::to_string(signature_.size()) + ", got " + std::to_string(data.size() - offset));
    return false;
  }
  std::copy(data.begin() + offset, data.begin() + offset + signature_.size(), signature_.begin());
  offset += signature_.size();

  // Deserialize hash (32 bytes)
  if (offset + hash_.size() > data.size())
  {
    utilities::log_error("Insufficient data for hash_. Expected " + std::to_string(hash_.size()) + ", got " + std::to_string(data.size() - offset));
    return false;
  }
  std::copy(data.begin() + offset, data.begin() + offset + hash_.size(), hash_.begin());
  offset += hash_.size();

  // Verify that all data has been consumed
  if (offset != data.size())
  {
    utilities::log_error("Extra data found after deserializing Transaction. Expected offset " + std::to_string(data.size()) + ", got " + std::to_string(offset));
    return false;
  }

  utilities::log_info("Transaction deserialized successfully.");
  return true;
}

bytes Transaction::get_data_to_sign() const
{
  bytes data;

  // Include sender public key (32 bytes)
  data.insert(data.end(), sender_public_key_.begin(), sender_public_key_.end());

  // Include recipient public key (32 bytes)
  data.insert(data.end(), recipient_public_key_.begin(), recipient_public_key_.end());

  // Include amount (8 bytes)
  bytes amount_bytes(reinterpret_cast<const byte *>(&amount_), reinterpret_cast<const byte *>(&amount_) + sizeof(uint64_t));
  data.insert(data.end(), amount_bytes.begin(), amount_bytes.end());

  // Include data size (8 bytes)
  uint64_t data_size = data_.size(); // Should be 0 for coinbase
  bytes data_size_bytes(reinterpret_cast<const byte *>(&data_size), reinterpret_cast<const byte *>(&data_size) + sizeof(uint64_t));
  data.insert(data.end(), data_size_bytes.begin(), data_size_bytes.end());

  // Include data (variable, 0 bytes for coinbase)
  data.insert(data.end(), data_.begin(), data_.end());

  return data;
}

void Transaction::compute_hash()
{
  bytes data = get_data_to_sign();
  data.insert(data.end(), signature_.begin(), signature_.end());
  hash_ = cryptography::sha256(data);
}

bool Transaction::verify() const
{
  // For coinbase transactions, skip signature verification
  if (is_coinbase_transaction())
  {
    return true;
  }

  bytes data_to_verify = get_data_to_sign();
  return cryptography::verify_signature(data_to_verify, signature_, sender_public_key_);
}

bool Transaction::is_coinbase_transaction() const
{
  // Determine if the transaction is a coinbase transaction
  // For example, check if sender_public_key_ is zeroed out
  return std::all_of(sender_public_key_.begin(), sender_public_key_.end(), [](byte b)
                     { return b == 0; });
}

// Equality operator
bool Transaction::operator==(const Transaction &other) const
{
  return sender_public_key_ == other.sender_public_key_ &&
         recipient_public_key_ == other.recipient_public_key_ &&
         amount_ == other.amount_ &&
         data_ == other.data_ &&
         signature_ == other.signature_ &&
         hash_ == other.hash_;
}
