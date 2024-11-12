#ifndef STORAGE_INTERFACE_HPP
#define STORAGE_INTERFACE_HPP

#include <string>
#include <vector>
#include <optional>
#include "../common/types.hpp"

template <typename BlockType>
class StorageInterface
{
public:
  virtual ~StorageInterface() = default;

  // Initializes the storage system
  virtual bool initialize(const std::string &data_directory) = 0;

  // Stores a block
  virtual bool store_block(const BlockType &block) = 0;

  // Retrieves a block by its hash
  virtual std::optional<BlockType> get_block(const Hash &block_hash) = 0;

  // Retrieves the latest block
  virtual std::optional<BlockType> get_latest_block() = 0;

  // Checks if a block exists
  virtual bool block_exists(const Hash &block_hash) = 0;

  // Closes the storage system
  virtual void close() = 0;
};

#endif // STORAGE_INTERFACE_HPP
