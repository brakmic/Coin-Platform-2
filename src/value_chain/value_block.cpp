#include "value_block.hpp"
#include "../common/utilities.hpp"
#include "../cryptography/cryptography.hpp"
#include <cstring>

ValueBlock::ValueBlock(const Hash &previous_hash,
                       const Hash &time_block_hash,
                       TimePoint time,
                       const std::vector<Transaction> &transactions,
                       const PublicKey &public_key)
    : previous_hash_(previous_hash),
      time_block_hash_(time_block_hash),
      time_(time),
      transactions_(transactions),
      public_key_(public_key)
{
  signature_.fill(0);
  compute_hash();
}

ValueBlock::ValueBlock()
    : time_(0)
{
  previous_hash_.fill(0);
  time_block_hash_.fill(0);
  public_key_.fill(0);
  signature_.fill(0);
  hash_.fill(0);
}

const Hash &ValueBlock::get_previous_hash() const
{
  return previous_hash_;
}

const Hash &ValueBlock::get_time_block_hash() const
{
  return time_block_hash_;
}

const TimePoint &ValueBlock::get_time() const
{
  return time_;
}

const std::vector<Transaction> &ValueBlock::get_transactions() const
{
  return transactions_;
}

const PublicKey &ValueBlock::get_public_key() const
{
  return public_key_;
}

const Signature &ValueBlock::get_signature() const
{
  return signature_;
}

const Hash &ValueBlock::get_hash() const
{
  return hash_;
}

void ValueBlock::set_signature(const Signature &signature)
{
  signature_ = signature;
  compute_hash();
}

// value_block.cpp

bytes ValueBlock::serialize() const
{
  bytes data;

  // Serialize previous hash
  data.insert(data.end(), previous_hash_.begin(), previous_hash_.end());

  // Serialize time block hash
  data.insert(data.end(), time_block_hash_.begin(), time_block_hash_.end());

  // Serialize time
  bytes time_bytes(reinterpret_cast<const byte *>(&time_), reinterpret_cast<const byte *>(&time_) + sizeof(TimePoint));
  data.insert(data.end(), time_bytes.begin(), time_bytes.end());

  // Serialize number of transactions
  uint64_t num_transactions = transactions_.size();
  bytes num_tx_bytes(reinterpret_cast<const byte *>(&num_transactions), reinterpret_cast<const byte *>(&num_transactions) + sizeof(uint64_t));
  data.insert(data.end(), num_tx_bytes.begin(), num_tx_bytes.end());

  // Serialize transactions
  for (const auto &tx : transactions_)
  {
    bytes tx_bytes = tx.serialize();
    // Serialize transaction size
    uint64_t tx_size = tx_bytes.size();
    bytes tx_size_bytes(reinterpret_cast<const byte *>(&tx_size), reinterpret_cast<const byte *>(&tx_size) + sizeof(uint64_t));
    data.insert(data.end(), tx_size_bytes.begin(), tx_size_bytes.end());
    // Insert transaction bytes
    data.insert(data.end(), tx_bytes.begin(), tx_bytes.end());
  }

  // Serialize public key
  data.insert(data.end(), public_key_.begin(), public_key_.end());

  // Serialize signature
  data.insert(data.end(), signature_.begin(), signature_.end());

  // Serialize hash
  data.insert(data.end(), hash_.begin(), hash_.end());

  return data;
}

bool ValueBlock::deserialize(const bytes &data)
{
  size_t offset = 0;

  // Deserialize previous hash
  if (offset + previous_hash_.size() > data.size())
    return false;
  std::copy(data.begin() + offset, data.begin() + offset + previous_hash_.size(), previous_hash_.begin());
  offset += previous_hash_.size();

  // Deserialize time block hash
  if (offset + time_block_hash_.size() > data.size())
    return false;
  std::copy(data.begin() + offset, data.begin() + offset + time_block_hash_.size(), time_block_hash_.begin());
  offset += time_block_hash_.size();

  // Deserialize time
  if (offset + sizeof(TimePoint) > data.size())
    return false;
  std::memcpy(&time_, &data[offset], sizeof(TimePoint));
  offset += sizeof(TimePoint);

  // Deserialize number of transactions
  uint64_t num_transactions = 0;
  if (offset + sizeof(uint64_t) > data.size())
    return false;
  std::memcpy(&num_transactions, &data[offset], sizeof(uint64_t));
  offset += sizeof(uint64_t);

  // Deserialize transactions
  transactions_.clear();
  for (uint64_t i = 0; i < num_transactions; ++i)
  {
    // Deserialize transaction size
    uint64_t tx_size = 0;
    if (offset + sizeof(uint64_t) > data.size())
      return false;
    std::memcpy(&tx_size, &data[offset], sizeof(uint64_t));
    offset += sizeof(uint64_t);

    // Deserialize transaction data
    if (offset + tx_size > data.size())
      return false;
    bytes tx_bytes(data.begin() + offset, data.begin() + offset + tx_size);
    offset += tx_size;

    Transaction tx;
    if (!tx.deserialize(tx_bytes))
    {
      utilities::log_error("Failed to deserialize transaction in ValueBlock.");
      return false;
    }
    transactions_.push_back(tx);
  }

  // Deserialize public key
  if (offset + public_key_.size() > data.size())
    return false;
  std::copy(data.begin() + offset, data.begin() + offset + public_key_.size(), public_key_.begin());
  offset += public_key_.size();

  // Deserialize signature
  if (offset + signature_.size() > data.size())
    return false;
  std::copy(data.begin() + offset, data.begin() + offset + signature_.size(), signature_.begin());
  offset += signature_.size();

  // Deserialize hash
  if (offset + hash_.size() > data.size())
    return false;
  std::copy(data.begin() + offset, data.begin() + offset + hash_.size(), hash_.begin());
  offset += hash_.size();
  utilities::log_info("ValueBlock deserialized successfully.");
  return true;
}

bytes ValueBlock::get_data_to_sign() const
{
  bytes data;

  // Include previous hash
  data.insert(data.end(), previous_hash_.begin(), previous_hash_.end());

  // Include time block hash
  data.insert(data.end(), time_block_hash_.begin(), time_block_hash_.end());

  // Include time
  bytes time_bytes(reinterpret_cast<const byte *>(&time_), reinterpret_cast<const byte *>(&time_) + sizeof(TimePoint));
  data.insert(data.end(), time_bytes.begin(), time_bytes.end());

  // Include number of transactions
  uint64_t num_transactions = transactions_.size();
  bytes num_tx_bytes(reinterpret_cast<const byte *>(&num_transactions), reinterpret_cast<const byte *>(&num_transactions) + sizeof(uint64_t));
  data.insert(data.end(), num_tx_bytes.begin(), num_tx_bytes.end());

  // Include transactions
  for (const auto &tx : transactions_)
  {
    bytes tx_bytes = tx.serialize();
    // Include transaction size
    uint64_t tx_size = tx_bytes.size();
    bytes tx_size_bytes(reinterpret_cast<const byte *>(&tx_size), reinterpret_cast<const byte *>(&tx_size) + sizeof(uint64_t));
    data.insert(data.end(), tx_size_bytes.begin(), tx_size_bytes.end());
    // Include transaction bytes
    data.insert(data.end(), tx_bytes.begin(), tx_bytes.end());
  }

  // Include public key
  data.insert(data.end(), public_key_.begin(), public_key_.end());

  return data;
}

void ValueBlock::compute_hash()
{
  bytes data = get_data_to_sign();
  data.insert(data.end(), signature_.begin(), signature_.end());
  hash_ = cryptography::sha256(data);
}
