// src/consensus/time_chain_consensus.cpp

#include "time_chain_consensus.hpp"
#include "../common/utilities.hpp"
#include "../cryptography/cryptography.hpp"
#include "../time_chain/time_block.hpp"
#include "../common/genesis_blocks.hpp"

TimeChainConsensus::TimeChainConsensus(
    std::shared_ptr<StorageInterface<TimeBlock>> storage,
    std::shared_ptr<NetworkManager> network_manager,
    const PrivateKey &private_key)
    : storage_(storage),
      network_manager_(network_manager),
      private_key_(private_key),
      rng_(std::random_device{}())
{
  public_key_ = cryptography::derive_public_key(private_key_);
}

bool TimeChainConsensus::initialize()
{
  // Check if TimeChain has any blocks
  auto latest_block_opt = storage_->get_latest_block();
  if (!latest_block_opt)
  {
    utilities::log_info("No existing TimeBlocks found. Loading genesis TimeBlock.");

    // Deserialize the predefined genesis block
    TimeBlock genesis_block;
    if (!genesis_block.deserialize(genesis::TIME_CHAIN_GENESIS_BLOCK_DATA))
    {
      utilities::log_error("Failed to deserialize the genesis TimeBlock data.");
      return false;
    }

    // Verify the genesis block's hash matches the precomputed hash
    if (genesis_block.get_hash() != genesis::TIME_CHAIN_GENESIS_HASH)
    {
      utilities::log_error("Genesis TimeBlock hash does not match the precomputed hash.");
      return false;
    }
    utilities::log_info("Genesis TimeBlock hash verified successfully.");

    // Store the genesis block
    if (!storage_->store_block(genesis_block))
    {
      utilities::log_error("Failed to store the genesis TimeBlock.");
      return false;
    }

    utilities::log_info("Genesis TimeBlock loaded and added to TimeChain.");
  }
  else
  {
    utilities::log_info("Existing TimeBlocks found. Skipping genesis block loading.");
  }

  utilities::log_info("TimeChainConsensus initialized.");
  return true;
}

bool TimeChainConsensus::validate_block(const TimeBlock &block)
{
  // Verify time
  if (!verify_time(block))
  {
    utilities::log_error("Block failed time verification.");
    return false;
  }

  // Verify signature
  if (!verify_signature(block))
  {
    utilities::log_error("Block failed signature verification.");
    return false;
  }

  // Additional validation if necessary
  return true;
}

bool TimeChainConsensus::is_eligible_to_produce_block()
{
  // Implement stochastic function
  std::uniform_int_distribution<uint64_t> distribution(0, UINT64_MAX);
  uint64_t random_number = distribution(rng_);
  uint64_t threshold = UINT64_MAX / 10; // Example threshold (10% chance)

  bool eligible = random_number < threshold;
  if (eligible)
  {
    utilities::log_info("Node is eligible to produce a TimeBlock.");
  }
  return eligible;
}

std::optional<TimeBlock> TimeChainConsensus::produce_block()
{
  if (!is_eligible_to_produce_block())
  {
    return std::nullopt;
  }

  // Get the latest block from storage
  auto latest_block_opt = storage_->get_latest_block();
  Hash previous_hash;

  if (latest_block_opt)
  {
    previous_hash = latest_block_opt->get_hash();
  }
  else
  {
    // This should not happen as we've ensured a genesis block exists in initialize()
    previous_hash.fill(0);
  }

  // Create a new TimeBlock
  TimePoint current_time = get_current_time();
  TimeBlock block(previous_hash, current_time, public_key_);

  // Sign the block
  bytes block_data = block.get_data_to_sign();
  Signature signature = cryptography::sign_message(block_data, private_key_);
  block.set_signature(signature);

  // Store the block
  if (storage_->store_block(block))
  {
    utilities::log_info("Produced and stored new TimeBlock.");
    return block;
  }
  else
  {
    utilities::log_error("Failed to store new TimeBlock.");
    return std::nullopt;
  }
}

void TimeChainConsensus::handle_block(const TimeBlock &block)
{
  if (validate_block(block))
  {
    // Store the block
    if (storage_->store_block(block))
    {
      utilities::log_info("Received and stored new TimeBlock.");

      // Additional actions, such as broadcasting the block to peers, can be added here
    }
    else
    {
      utilities::log_error("Failed to store received TimeBlock.");
    }
  }
  else
  {
    utilities::log_error("Invalid TimeBlock received.");
  }
}

bool TimeChainConsensus::verify_time(const TimeBlock &block)
{
  // Get the latest block to compare times
  auto latest_block_opt = storage_->get_latest_block();
  if (latest_block_opt)
  {
    TimePoint latest_time = latest_block_opt->get_time();
    if (block.get_time() <= latest_time)
    {
      utilities::log_error("Block time is not increasing.");
      return false;
    }
  }
  // Additional time verification logic if needed
  return true;
}

bool TimeChainConsensus::verify_signature(const TimeBlock &block)
{
  bytes data_to_verify = block.get_data_to_sign();
  const Signature &signature = block.get_signature();
  const PublicKey &block_public_key = block.get_public_key();

  return cryptography::verify_signature(data_to_verify, signature, block_public_key);
}

uint64_t TimeChainConsensus::get_current_time()
{
  // Return current time as TimePoint (nanoseconds since epoch)
  return utilities::get_current_time();
}
