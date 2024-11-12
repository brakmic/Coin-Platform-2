#include "time_chain.hpp"
#include "../common/utilities.hpp"

TimeChain::TimeChain(std::shared_ptr<StorageInterface<TimeBlock>> storage)
    : storage_(storage) {}

bool TimeChain::add_block(const TimeBlock& block) {
    // Store the block
    if (storage_->store_block(block)) {
        utilities::log_info("Added TimeBlock to TimeChain.");
        return true;
    } else {
        utilities::log_error("Failed to add TimeBlock to TimeChain.");
        return false;
    }
}

std::optional<TimeBlock> TimeChain::get_latest_block() const {
    return storage_->get_latest_block();
}

Hash TimeChain::get_latest_block_hash() const {
    auto latest_block_opt = get_latest_block();
    if (latest_block_opt) {
        return latest_block_opt->get_hash();
    } else {
        // Return zero-filled hash if no blocks exist
        Hash zero_hash;
        zero_hash.fill(0);
        return zero_hash;
    }
}

bool TimeChain::block_exists(const Hash& block_hash) const {
    return storage_->block_exists(block_hash);
}
