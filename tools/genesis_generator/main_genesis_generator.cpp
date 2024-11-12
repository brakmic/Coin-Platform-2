#include "genesis_generator.hpp"
#include "../../src/common/types.hpp"
#include "../../src/common/utilities.hpp"
#include "../../src/cryptography/cryptography.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./genesis_generator <private_key_hex>" << std::endl;
        return 1;
    }

    // Convert private key hex string to bytes
    std::string private_key_hex = argv[1];
    bytes private_key_bytes = utilities::hex_to_bytes(private_key_hex);
    if (private_key_bytes.size() != PRIVATE_KEY_SIZE)
    {
        std::cerr << "Invalid private key size." << std::endl;
        return 1;
    }

    PrivateKey private_key;
    std::copy(private_key_bytes.begin(), private_key_bytes.end(), private_key.begin());

    genesis::GenesisGenerator generator(private_key);

    // Generate genesis blocks
    TimeBlock time_genesis = generator.generate_time_chain_genesis_block();
    ValueBlock value_genesis = generator.generate_value_chain_genesis_block(time_genesis);

    // Serialize genesis blocks
    bytes time_block_data = time_genesis.serialize();
    bytes value_block_data = value_genesis.serialize();

    // Write the genesis block data and hashes to a single header file
    std::ofstream genesis_file("/Users/brakmic/projects/cpp/coin_platform2/src/common/genesis_blocks.hpp");
    if (!genesis_file.is_open())
    {
        std::cerr << "Failed to open /Users/brakmic/projects/cpp/coin_platform2/src/common/genesis_blocks.hpp for writing." << std::endl;
        return 1;
    }

    genesis_file << "#ifndef GENESIS_BLOCKS_HPP\n";
    genesis_file << "#define GENESIS_BLOCKS_HPP\n\n";
    genesis_file << "#include \"types.hpp\"\n\n";
    genesis_file << "namespace genesis {\n\n";

    // Write TimeChain Genesis Block Data
    genesis_file << "const bytes TIME_CHAIN_GENESIS_BLOCK_DATA = {";
    for (size_t i = 0; i < time_block_data.size(); ++i)
    {
        if (i % 16 == 0)
            genesis_file << "\n    ";
        genesis_file << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(time_block_data[i]);
        if (i != time_block_data.size() - 1)
            genesis_file << ", ";
    }
    genesis_file << "\n};\n\n";

    // Write ValueChain Genesis Block Data
    genesis_file << "const bytes VALUE_CHAIN_GENESIS_BLOCK_DATA = {";
    for (size_t i = 0; i < value_block_data.size(); ++i)
    {
        if (i % 16 == 0)
            genesis_file << "\n    ";
        genesis_file << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value_block_data[i]);
        if (i != value_block_data.size() - 1)
            genesis_file << ", ";
    }
    genesis_file << "\n};\n\n";

    // Write TimeChain Genesis Hash
    Hash time_genesis_hash = time_genesis.get_hash();
    genesis_file << "const Hash TIME_CHAIN_GENESIS_HASH = {";
    for (size_t i = 0; i < time_genesis_hash.size(); ++i)
    {
        if (i % 8 == 0)
            genesis_file << "\n    ";
        genesis_file << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(time_genesis_hash[i]);
        if (i != time_genesis_hash.size() - 1)
            genesis_file << ", ";
    }
    genesis_file << "\n};\n\n";

    // Write ValueChain Genesis Hash
    Hash value_genesis_hash = value_genesis.get_hash();
    genesis_file << "const Hash VALUE_CHAIN_GENESIS_HASH = {";
    for (size_t i = 0; i < value_genesis_hash.size(); ++i)
    {
        if (i % 8 == 0)
            genesis_file << "\n    ";
        genesis_file << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value_genesis_hash[i]);
        if (i != value_genesis_hash.size() - 1)
            genesis_file << ", ";
    }
    genesis_file << "\n};\n\n";

    genesis_file << "} // namespace genesis\n";
    genesis_file << "#endif // GENESIS_BLOCKS_HPP\n";
    genesis_file.close();

    std::cout << "Genesis block data and hashes have been written to src/common/genesis_blocks.hpp" << std::endl;

    // Output the genesis block hashes to the console
    std::cout << "Time Chain Genesis Block Hash: " << utilities::bytes_to_hex(bytes(time_genesis_hash.begin(), time_genesis_hash.end())) << std::endl;
    std::cout << "Value Chain Genesis Block Hash: " << utilities::bytes_to_hex(bytes(value_genesis_hash.begin(), value_genesis_hash.end())) << std::endl;

    return 0;
}
