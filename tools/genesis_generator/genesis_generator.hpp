#ifndef GENESIS_GENERATOR_HPP
#define GENESIS_GENERATOR_HPP

#include "../../src/common/types.hpp"
#include "../../src/cryptography/cryptography.hpp"
#include "../../src/time_chain/time_block.hpp"
#include "../../src/value_chain/value_block.hpp"

namespace genesis
{

  class GenesisGenerator
  {
  public:
    explicit GenesisGenerator(const PrivateKey &private_key);

    TimeBlock generate_time_chain_genesis_block();
    ValueBlock generate_value_chain_genesis_block(const TimeBlock &time_chain_genesis_block);

    Hash get_time_chain_genesis_hash(const TimeBlock &block) const;
    Hash get_value_chain_genesis_hash(const ValueBlock &block) const;

  private:
    PrivateKey private_key_;
    PublicKey public_key_;
  };

} // namespace genesis

#endif // GENESIS_GENERATOR_HPP
