#include "time_block.hpp"
#include "../common/utilities.hpp"
#include "../cryptography/cryptography.hpp"
#include <cstring>

TimeBlock::TimeBlock(const Hash &previous_hash, TimePoint time, const PublicKey &public_key)
    : previous_hash_(previous_hash), time_(time), public_key_(public_key)
{
  signature_.fill(0);
  compute_hash();
}

TimeBlock::TimeBlock() : time_(0)
{
  previous_hash_.fill(0);
  public_key_.fill(0);
  signature_.fill(0);
  hash_.fill(0);
}

const Hash &TimeBlock::get_previous_hash() const
{
  return previous_hash_;
}

const TimePoint &TimeBlock::get_time() const
{
  return time_;
}

const PublicKey &TimeBlock::get_public_key() const
{
  return public_key_;
}

const Signature &TimeBlock::get_signature() const
{
  return signature_;
}

const Hash &TimeBlock::get_hash() const
{
  return hash_;
}

void TimeBlock::set_signature(const Signature &signature)
{
  signature_ = signature;
  compute_hash();
}

// time_block.cpp

bytes TimeBlock::serialize() const
{
  bytes data;
  data.insert(data.end(), previous_hash_.begin(), previous_hash_.end());

  bytes time_bytes(reinterpret_cast<const byte *>(&time_), reinterpret_cast<const byte *>(&time_) + sizeof(TimePoint));
  data.insert(data.end(), time_bytes.begin(), time_bytes.end());

  data.insert(data.end(), public_key_.begin(), public_key_.end());
  data.insert(data.end(), signature_.begin(), signature_.end());
  data.insert(data.end(), hash_.begin(), hash_.end());
  return data;
}

bool TimeBlock::deserialize(const bytes &data)
{
  size_t expected_size = previous_hash_.size() + sizeof(TimePoint) + public_key_.size() + signature_.size() + hash_.size();
  if (data.size() != expected_size)
  {
    utilities::log_error("Data size mismatch during deserialization.");
    return false;
  }
  auto it = data.begin();

  std::copy(it, it + previous_hash_.size(), previous_hash_.begin());
  it += previous_hash_.size();

  std::memcpy(&time_, &(*it), sizeof(TimePoint));
  it += sizeof(TimePoint);

  std::copy(it, it + public_key_.size(), public_key_.begin());
  it += public_key_.size();

  std::copy(it, it + signature_.size(), signature_.begin());
  it += signature_.size();

  std::copy(it, it + hash_.size(), hash_.begin());
  utilities::log_info("TimeBlock deserialized successfully.");
  return true;
}

bytes TimeBlock::get_data_to_sign() const
{
  bytes data;
  data.insert(data.end(), previous_hash_.begin(), previous_hash_.end());

  bytes time_bytes(reinterpret_cast<const byte *>(&time_), reinterpret_cast<const byte *>(&time_) + sizeof(TimePoint));
  data.insert(data.end(), time_bytes.begin(), time_bytes.end());

  data.insert(data.end(), public_key_.begin(), public_key_.end());
  return data;
}

void TimeBlock::compute_hash()
{
  bytes data = get_data_to_sign();
  data.insert(data.end(), signature_.begin(), signature_.end());
  hash_ = cryptography::sha256(data);
}
