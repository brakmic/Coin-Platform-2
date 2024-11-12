// genesis_reference.hpp

#ifndef GENESIS_REFERENCE_HPP
#define GENESIS_REFERENCE_HPP

#include <string>
#include <vector>
#include "types.hpp"

namespace genesis_reference
{

  // The reference text used to derive the genesis block's hash.
  // Example: Bitcoin's genesis block references a newspaper headline.
  const std::string TIME_CHAIN_REFERENCE = "Chancellor on brink of second bailout for banks";
  const std::string VALUE_CHAIN_REFERENCE = "First ValueChain Genesis Block Reference";

  // Function to get the reference as bytes
  inline std::vector<byte> string_to_bytes(const std::string &str)
  {
    return std::vector<byte>(str.begin(), str.end());
  }

} // namespace genesis_reference

#endif // GENESIS_REFERENCE_HPP
