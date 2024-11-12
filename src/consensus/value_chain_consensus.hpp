#ifndef VALUE_CHAIN_CONSENSUS_HPP
#define VALUE_CHAIN_CONSENSUS_HPP

#include "consensus_interface.hpp"
#include "../value_chain/value_block.hpp"
#include "../value_chain/transaction.hpp"
#include "../storage/storage_interface.hpp"
#include "../networking/network_manager.hpp"
#include "../cryptography/cryptography.hpp"
#include "../time_chain/time_chain.hpp"
#include <memory>
#include <random>
#include <mutex>
#include <vector>

class ValueChainConsensus : public ConsensusInterface<ValueBlock>
{
public:
  ValueChainConsensus(
      std::shared_ptr<StorageInterface<ValueBlock>> storage,
      std::shared_ptr<NetworkManager> network_manager,
      std::shared_ptr<TimeChain> time_chain,
      const PrivateKey &private_key);

  bool initialize() override;
  bool validate_block(const ValueBlock &block) override;
  bool is_eligible_to_produce_block() override;
  std::optional<ValueBlock> produce_block() override;
  void handle_block(const ValueBlock &block) override;

  // Method to add a transaction to the transaction pool
  void add_transaction(const Transaction &transaction);

private:
  std::shared_ptr<StorageInterface<ValueBlock>> storage_;
  std::shared_ptr<NetworkManager> network_manager_;
  std::shared_ptr<TimeChain> time_chain_;
  PrivateKey private_key_;
  PublicKey public_key_;

  // Random number generator for stochastic function
  std::mt19937_64 rng_;

  // Transaction pool
  std::mutex transaction_pool_mutex_;
  std::vector<Transaction> transaction_pool_;

  // Helper methods
  bool verify_time_reference(const ValueBlock &block);
  bool verify_transactions(const ValueBlock &block);
  std::vector<Transaction> gather_transactions();
  bool verify_signature(const ValueBlock &block);
  uint64_t get_current_time();
};

#endif // VALUE_CHAIN_CONSENSUS_HPP
