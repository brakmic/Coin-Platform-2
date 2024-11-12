// src/consensus/value_chain_consensus.cpp

#include "value_chain_consensus.hpp"
#include "../common/utilities.hpp"
#include "../cryptography/cryptography.hpp"
#include "../value_chain/transaction.hpp"
#include "../value_chain/value_block.hpp"
#include "../common/genesis_blocks.hpp"

ValueChainConsensus::ValueChainConsensus(
    std::shared_ptr<StorageInterface<ValueBlock>> storage,
    std::shared_ptr<NetworkManager> network_manager,
    std::shared_ptr<TimeChain> time_chain,
    const PrivateKey &private_key)
    : storage_(storage),
      network_manager_(network_manager),
      time_chain_(time_chain),
      private_key_(private_key),
      rng_(std::random_device{}())
{
  public_key_ = cryptography::derive_public_key(private_key_);
}

bool ValueChainConsensus::initialize()
{
    // Check if ValueChain has any blocks
    auto latest_block_opt = storage_->get_latest_block();
    if (!latest_block_opt)
    {
        utilities::log_info("No existing ValueBlocks found. Loading genesis ValueBlock.");

        // Deserialize the predefined genesis block
        ValueBlock genesis_block;
        if (!genesis_block.deserialize(genesis::VALUE_CHAIN_GENESIS_BLOCK_DATA))
        {
            utilities::log_error("Failed to deserialize the genesis ValueBlock data.");
            return false;
        }

        // Verify the genesis block's hash matches the precomputed hash
        if (genesis_block.get_hash() != genesis::VALUE_CHAIN_GENESIS_HASH)
        {
            utilities::log_error("Genesis ValueBlock hash does not match the precomputed hash.");
            return false;
        }
        utilities::log_info("Genesis ValueBlock hash verified successfully.");

        // Store the genesis block
        if (!storage_->store_block(genesis_block))
        {
            utilities::log_error("Failed to store the genesis ValueBlock.");
            return false;
        }

        utilities::log_info("Genesis ValueBlock loaded and added to ValueChain.");
    }
    else
    {
        utilities::log_info("Existing ValueBlocks found. Skipping genesis block loading.");
    }

    utilities::log_info("ValueChainConsensus initialized.");
    return true;
}

bool ValueChainConsensus::validate_block(const ValueBlock &block)
{
  // Verify time reference
  if (!verify_time_reference(block))
  {
    utilities::log_error("Block failed time reference verification.");
    return false;
  }

  // Verify transactions
  if (!verify_transactions(block))
  {
    utilities::log_error("Block contains invalid transactions.");
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

bool ValueChainConsensus::is_eligible_to_produce_block()
{
  // Implement stochastic function based on time from TimeChain
  uint64_t current_time = get_current_time();
  std::uniform_int_distribution<uint64_t> distribution(0, UINT64_MAX);
  uint64_t random_number = distribution(rng_);
  uint64_t threshold = UINT64_MAX / 100; // Example threshold (1% chance)

  // Incorporate current time into the stochastic function (optional)
  uint64_t adjusted_threshold = threshold + (current_time % threshold);

  bool eligible = random_number < adjusted_threshold;
  if (eligible)
  {
    utilities::log_info("Node is eligible to produce a ValueBlock.");
  }
  return eligible;
}

std::optional<ValueBlock> ValueChainConsensus::produce_block()
{
  if (!is_eligible_to_produce_block())
  {
    return std::nullopt;
  }

  // Gather transactions to include in the block
  std::vector<Transaction> transactions = gather_transactions();

  if (transactions.empty())
  {
    // No transactions to include, so we don't produce a block
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
    // Genesis block scenario
    previous_hash.fill(0);
  }

  // Get the latest time from the TimeChain
  TimePoint current_time = get_current_time();
  Hash time_block_hash = time_chain_->get_latest_block_hash();

  // Create a new ValueBlock
  ValueBlock block(previous_hash, time_block_hash, current_time, transactions, public_key_);

  // Sign the block
  bytes block_data = block.get_data_to_sign();
  Signature signature = cryptography::sign_message(block_data, private_key_);
  block.set_signature(signature);

  // Store the block
  if (storage_->store_block(block))
  {
    utilities::log_info("Produced and stored new ValueBlock.");
    return block;
  }
  else
  {
    utilities::log_error("Failed to store new ValueBlock.");
    return std::nullopt;
  }
}

void ValueChainConsensus::handle_block(const ValueBlock &block)
{
  if (validate_block(block))
  {
    // Store the block
    if (storage_->store_block(block))
    {
      utilities::log_info("Received and stored new ValueBlock.");

      // Remove transactions included in the block from the transaction pool
      {
        std::lock_guard<std::mutex> lock(transaction_pool_mutex_);
        for (const auto &tx : block.get_transactions())
        {
          auto it = std::remove(transaction_pool_.begin(), transaction_pool_.end(), tx);
          transaction_pool_.erase(it, transaction_pool_.end());
        }
      }

      // Optionally, broadcast the block to peers
      // network_manager_->broadcast_data(block.serialize());
    }
    else
    {
      utilities::log_error("Failed to store received ValueBlock.");
    }
  }
  else
  {
    utilities::log_error("Invalid ValueBlock received.");
  }
}

void ValueChainConsensus::add_transaction(const Transaction &transaction)
{
  std::lock_guard<std::mutex> lock(transaction_pool_mutex_);
  transaction_pool_.push_back(transaction);
  utilities::log_info("Transaction added to the pool.");
}

std::vector<Transaction> ValueChainConsensus::gather_transactions()
{
  std::vector<Transaction> transactions;

  // Add transactions from the transaction pool
  {
    std::lock_guard<std::mutex> lock(transaction_pool_mutex_);
    if (!transaction_pool_.empty())
    {
      transactions.insert(transactions.end(), transaction_pool_.begin(), transaction_pool_.end());
      // Clear the transaction pool after gathering
      transaction_pool_.clear();
    }
  }

  if (transactions.empty())
  {
    // No transactions to include
    return {};
  }

  // Create a coinbase transaction
  // For a coinbase transaction, the sender is typically a special value
  // Since there's no actual sender, we use a zeroed public key
  PublicKey coinbase_sender_public_key = {}; // Zero-initialized public key

  // The recipient is the miner's public key
  PublicKey recipient_public_key = public_key_;

  // Amount to be rewarded
  uint64_t amount = 50;

  // Optional data (can include reference text if desired)
  // For simplicity, we leave it empty or include specific data
  std::vector<byte> data = {}; // You can include reference data here if needed

  // Create the coinbase transaction
  Transaction coinbase_transaction(coinbase_sender_public_key, recipient_public_key, amount, data);

  // Sign the transaction with the miner's private key
  bytes tx_data_to_sign = coinbase_transaction.get_data_to_sign();
  Signature signature = cryptography::sign_message(tx_data_to_sign, private_key_);
  coinbase_transaction.set_signature(signature);

  // Insert the coinbase transaction at the beginning
  transactions.insert(transactions.begin(), coinbase_transaction);

  return transactions;
}

bool ValueChainConsensus::verify_time_reference(const ValueBlock &block)
{
  // Verify that the block references a valid TimeChain block
  Hash referenced_time_block_hash = block.get_time_block_hash();
  if (!time_chain_->block_exists(referenced_time_block_hash))
  {
    utilities::log_error("Referenced TimeBlock does not exist.");
    return false;
  }

  // Ensure the time is not outdated
  TimePoint block_time = block.get_time();
  TimePoint current_time = get_current_time();
  if (block_time > current_time)
  {
    utilities::log_error("Block time is in the future.");
    return false;
  }

  return true;
}

bool ValueChainConsensus::verify_transactions(const ValueBlock &block)
{
  // Implement transaction verification logic
  for (const auto &transaction : block.get_transactions())
  {
    if (!transaction.verify())
    {
      utilities::log_error("Invalid transaction detected.");
      return false;
    }
    // Additional checks, such as double-spending prevention, can be added here
  }
  return true;
}

bool ValueChainConsensus::verify_signature(const ValueBlock &block)
{
  bytes data_to_verify = block.get_data_to_sign();
  const Signature &signature = block.get_signature();
  const PublicKey &block_public_key = block.get_public_key();

  return cryptography::verify_signature(data_to_verify, signature, block_public_key);
}

uint64_t ValueChainConsensus::get_current_time()
{
  // Retrieve the current time from the TimeChain
  auto latest_time_block_opt = time_chain_->get_latest_block();
  if (latest_time_block_opt)
  {
    return latest_time_block_opt->get_time();
  }
  else
  {
    utilities::log_error("Failed to retrieve current time from TimeChain.");
    return 0;
  }
}
