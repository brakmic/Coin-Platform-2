// genesis_generator.cpp

#include "genesis_generator.hpp"
#include "../../src/common/types.hpp"
#include "../../src/common/genesis_reference.hpp"
#include "../../src/common/utilities.hpp"
#include "../../src/time_chain/time_block.hpp"
#include "../../src/value_chain/value_block.hpp"
#include "../../src/cryptography/cryptography.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>

namespace genesis
{

    GenesisGenerator::GenesisGenerator(const PrivateKey &private_key)
        : private_key_(private_key)
    {
        public_key_ = cryptography::derive_public_key(private_key_);
    }

    TimeBlock GenesisGenerator::generate_time_chain_genesis_block()
    {
        // Initialize previous hash with zeros for genesis block
        Hash previous_hash;
        previous_hash.fill(0);

        // Use the defined GENESIS_TIME
        TimePoint genesis_time = GENESIS_TIME;

        // Create the genesis TimeBlock
        TimeBlock genesis_block(previous_hash, genesis_time, public_key_);

        // Sign the genesis block
        bytes data_to_sign = genesis_block.get_data_to_sign();
        Signature signature = cryptography::sign_message(data_to_sign, private_key_);
        genesis_block.set_signature(signature);

        return genesis_block;
    }

    ValueBlock GenesisGenerator::generate_value_chain_genesis_block(const TimeBlock &time_chain_genesis_block)
    {
        // Initialize previous hash with zeros for genesis block
        Hash previous_hash;
        previous_hash.fill(0);

        // Reference the TimeBlock's hash
        Hash time_block_hash = time_chain_genesis_block.get_hash();

        // Use the defined GENESIS_TIME
        TimePoint genesis_time = GENESIS_TIME;

        // Create a coinbase transaction with the reference text
        std::vector<byte> reference_data = genesis_reference::string_to_bytes(genesis_reference::VALUE_CHAIN_REFERENCE);
        Transaction coinbase_tx(PublicKey{}, public_key_, 50, reference_data);

        // Sign the coinbase transaction
        bytes tx_data_to_sign = coinbase_tx.get_data_to_sign();
        Signature tx_signature = cryptography::sign_message(tx_data_to_sign, private_key_);
        coinbase_tx.set_signature(tx_signature);

        // Include the coinbase transaction in the genesis ValueBlock
        std::vector<Transaction> transactions = {coinbase_tx};

        // Create the genesis ValueBlock
        ValueBlock genesis_value_block(previous_hash, time_block_hash, genesis_time, transactions, public_key_);

        // Sign the genesis ValueBlock
        bytes value_block_data_to_sign = genesis_value_block.get_data_to_sign();
        Signature block_signature = cryptography::sign_message(value_block_data_to_sign, private_key_);
        genesis_value_block.set_signature(block_signature);

        return genesis_value_block;
    }

} // namespace genesis
