#include "value_chain.hpp"
#include "../common/utilities.hpp"

ValueChain::ValueChain(std::shared_ptr<StorageInterface<ValueBlock>> storage)
    : storage_(storage) {}

bool ValueChain::add_block(const ValueBlock& block) {
    // Store the block
    if (storage_->store_block(block)) {
        utilities::log_info("Added ValueBlock to ValueChain.");
        return true;
    } else {
        utilities::log_error("Failed to add ValueBlock to ValueChain.");
        return false;
    }
}

std::optional<ValueBlock> ValueChain::get_latest_block() const {
    return storage_->get_latest_block();
}

Hash ValueChain::get_latest_block_hash() const {
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

bool ValueChain::block_exists(const Hash& block_hash) const {
    return storage_->block_exists(block_hash);
}

std::optional<ValueBlock> ValueChain::get_block(const Hash& block_hash) const {
    return storage_->get_block(block_hash);
}
