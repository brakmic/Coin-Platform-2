#ifndef VALUE_CHAIN_HPP
#define VALUE_CHAIN_HPP

#include "value_block.hpp"
#include "../storage/storage_interface.hpp"
#include <memory>
#include <optional>

class ValueChain {
public:
    explicit ValueChain(std::shared_ptr<StorageInterface<ValueBlock>> storage);

    // Adds a block to the chain after validation
    bool add_block(const ValueBlock& block);

    // Retrieves the latest block
    std::optional<ValueBlock> get_latest_block() const;

    // Retrieves the hash of the latest block
    Hash get_latest_block_hash() const;

    // Checks if a block exists in the chain
    bool block_exists(const Hash& block_hash) const;

    // Retrieves a block by its hash
    std::optional<ValueBlock> get_block(const Hash& block_hash) const;

private:
    std::shared_ptr<StorageInterface<ValueBlock>> storage_;
};

#endif // VALUE_CHAIN_HPP
