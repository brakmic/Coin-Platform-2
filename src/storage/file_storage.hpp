#ifndef FILE_STORAGE_HPP
#define FILE_STORAGE_HPP

#include "storage_interface.hpp"
#include <string>
#include <mutex>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

template <typename BlockType>
class FileStorage : public StorageInterface<BlockType> {
public:
    FileStorage();
    ~FileStorage();

    bool initialize(const std::string& data_directory) override;
    bool store_block(const BlockType& block) override;
    std::optional<BlockType> get_block(const Hash& block_hash) override;
    std::optional<BlockType> get_latest_block() override;
    bool block_exists(const Hash& block_hash) override;
    void close() override;

private:
    std::string data_directory_;
    std::mutex storage_mutex_;

    // Helper methods
    std::string get_block_filename(const Hash& block_hash) const;
    bool write_block_to_file(const BlockType& block, const std::string& filename);
    std::optional<BlockType> read_block_from_file(const std::string& filename);
};

#include "file_storage.tpp"

#endif // FILE_STORAGE_HPP
