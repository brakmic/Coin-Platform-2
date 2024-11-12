#ifndef TIME_CHAIN_HPP
#define TIME_CHAIN_HPP

#include "time_block.hpp"
#include "../storage/storage_interface.hpp"
#include <memory>
#include <optional>

class TimeChain {
public:
    explicit TimeChain(std::shared_ptr<StorageInterface<TimeBlock>> storage);

    // Adds a block to the chain after validation
    bool add_block(const TimeBlock& block);

    // Retrieves the latest block
    std::optional<TimeBlock> get_latest_block() const;

    // Retrieves the hash of the latest block
    Hash get_latest_block_hash() const;

    // Checks if a block exists in the chain
    bool block_exists(const Hash& block_hash) const;

private:
    std::shared_ptr<StorageInterface<TimeBlock>> storage_;
};

#endif // TIME_CHAIN_HPP
