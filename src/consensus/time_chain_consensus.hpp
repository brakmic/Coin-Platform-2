#ifndef TIME_CHAIN_CONSENSUS_HPP
#define TIME_CHAIN_CONSENSUS_HPP

#include "consensus_interface.hpp"
#include "../time_chain/time_block.hpp"
#include "../storage/storage_interface.hpp"
#include "../networking/network_manager.hpp"
#include "../cryptography/cryptography.hpp"
#include <memory>
#include <random>

class TimeChainConsensus : public ConsensusInterface<TimeBlock>
{
public:
  TimeChainConsensus(
      std::shared_ptr<StorageInterface<TimeBlock>> storage,
      std::shared_ptr<NetworkManager> network_manager,
      const PrivateKey &private_key);

  bool initialize() override;
  bool validate_block(const TimeBlock &block) override;
  bool is_eligible_to_produce_block() override;
  std::optional<TimeBlock> produce_block() override;
  void handle_block(const TimeBlock &block) override;

private:
  std::shared_ptr<StorageInterface<TimeBlock>> storage_;
  std::shared_ptr<NetworkManager> network_manager_;
  PrivateKey private_key_;
  PublicKey public_key_;

  // Random number generator for stochastic function
  std::mt19937_64 rng_;

  // Helper methods
  bool verify_time(const TimeBlock &block);
  bool verify_signature(const TimeBlock &block);
  uint64_t get_current_time();
};

#endif // TIME_CHAIN_CONSENSUS_HPP
