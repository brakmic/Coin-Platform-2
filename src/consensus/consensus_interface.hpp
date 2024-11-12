#ifndef CONSENSUS_INTERFACE_HPP
#define CONSENSUS_INTERFACE_HPP

#include <optional>

template <typename BlockType>
class ConsensusInterface {
public:
    virtual ~ConsensusInterface() = default;

    // Initializes the consensus mechanism
    virtual bool initialize() = 0;

    // Validates a received block
    virtual bool validate_block(const BlockType& block) = 0;

    // Checks if the node is eligible to produce a block
    virtual bool is_eligible_to_produce_block() = 0;

    // Produces a new block if eligible
    virtual std::optional<BlockType> produce_block() = 0;

    // Handles a block received from the network
    virtual void handle_block(const BlockType& block) = 0;
};

#endif // CONSENSUS_INTERFACE_HPP
