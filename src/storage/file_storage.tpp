#ifndef FILE_STORAGE_TPP
#define FILE_STORAGE_TPP

#include "file_storage.hpp"
#include "../common/utilities.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>

template <typename BlockType>
FileStorage<BlockType>::FileStorage() : data_directory_("") {}

template <typename BlockType>
FileStorage<BlockType>::~FileStorage() {
    close();
}

template <typename BlockType>
bool FileStorage<BlockType>::initialize(const std::string& data_directory) {
    std::lock_guard<std::mutex> lock(storage_mutex_);
    data_directory_ = data_directory;
    try {
        if (!fs::exists(data_directory_)) {
            fs::create_directories(data_directory_);
            utilities::log_info("Created storage directory: " + data_directory_);
        } else {
            utilities::log_info("Storage directory already exists: " + data_directory_);
        }
        return true;
    } catch (const fs::filesystem_error& e) {
        utilities::log_error("Failed to initialize storage: " + std::string(e.what()));
        return false;
    }
}

template <typename BlockType>
bool FileStorage<BlockType>::store_block(const BlockType& block) {
    std::lock_guard<std::mutex> lock(storage_mutex_);
    std::string filename = get_block_filename(block.get_hash());
    bool write_success = write_block_to_file(block, filename);
    if (write_success) {
        utilities::log_info("Stored block: " + filename);
    } else {
        utilities::log_error("Failed to store block: " + filename);
    }
    return write_success;
}

template <typename BlockType>
std::optional<BlockType> FileStorage<BlockType>::get_block(const Hash& block_hash) {
    std::lock_guard<std::mutex> lock(storage_mutex_);
    std::string filename = get_block_filename(block_hash);
    if (fs::exists(filename)) {
        utilities::log_info("Retrieving block: " + filename);
        auto block_opt = read_block_from_file(filename);
        if (block_opt) {
            utilities::log_info("Successfully retrieved block: " + filename);
        } else {
            utilities::log_error("Failed to deserialize block: " + filename);
        }
        return block_opt;
    }
    utilities::log_error("Block does not exist: " + filename);
    return std::nullopt;
}

template <typename BlockType>
std::optional<BlockType> FileStorage<BlockType>::get_latest_block() {
    std::lock_guard<std::mutex> lock(storage_mutex_);
    std::string latest_block_file = data_directory_ + "/latest.block";
    if (fs::exists(latest_block_file)) {
        utilities::log_info("Retrieving latest block: " + latest_block_file);
        auto block_opt = read_block_from_file(latest_block_file);
        if (block_opt) {
            utilities::log_info("Successfully retrieved latest block.");
        } else {
            utilities::log_error("Failed to deserialize latest block.");
        }
        return block_opt;
    }
    utilities::log_error("Latest block file does not exist: " + latest_block_file);
    return std::nullopt;
}

template <typename BlockType>
bool FileStorage<BlockType>::block_exists(const Hash& block_hash) {
    std::lock_guard<std::mutex> lock(storage_mutex_);
    std::string filename = get_block_filename(block_hash);
    bool exists = fs::exists(filename);
    if (exists) {
        utilities::log_info("Block exists: " + filename);
    } else {
        utilities::log_info("Block does not exist: " + filename);
    }
    return exists;
}

template <typename BlockType>
void FileStorage<BlockType>::close() {
    std::lock_guard<std::mutex> lock(storage_mutex_);
    // Any necessary cleanup can be done here
    utilities::log_info("Storage system closed.");
}

template <typename BlockType>
std::string FileStorage<BlockType>::get_block_filename(const Hash& block_hash) const {
    // Convert hash to hex string for filename
    std::ostringstream oss;
    for (const auto& byte : block_hash) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    std::string hash_hex = oss.str();
    return data_directory_ + "/" + hash_hex + ".block";
}

template <typename BlockType>
bool FileStorage<BlockType>::write_block_to_file(const BlockType& block, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        utilities::log_error("Failed to open file for writing: " + filename);
        return false;
    }

    bytes serialized_block = block.serialize();
    file.write(reinterpret_cast<const char*>(serialized_block.data()), serialized_block.size());
    if (!file.good()) {
        utilities::log_error("Failed to write data to file: " + filename);
        file.close();
        return false;
    }
    file.close();
    utilities::log_info("Block written to file: " + filename);

    // Update latest block
    std::string latest_block_file = data_directory_ + "/latest.block";
    try {
        fs::copy_file(filename, latest_block_file, fs::copy_options::overwrite_existing);
        utilities::log_info("Updated latest block: " + latest_block_file);
    } catch (const fs::filesystem_error& e) {
        utilities::log_error("Failed to update latest block: " + std::string(e.what()));
        return false;
    }

    return true;
}

template <typename BlockType>
std::optional<BlockType> FileStorage<BlockType>::read_block_from_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        utilities::log_error("Failed to open file for reading: " + filename);
        return std::nullopt;
    }

    // Determine file size
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read file content
    bytes serialized_block(file_size);
    file.read(reinterpret_cast<char*>(serialized_block.data()), file_size);
    if (!file.good()) {
        utilities::log_error("Failed to read data from file: " + filename);
        file.close();
        return std::nullopt;
    }
    file.close();
    utilities::log_info("Read data from file: " + filename);

    // Deserialize the block
    BlockType block;
    if (block.deserialize(serialized_block)) {
        utilities::log_info("Deserialized block from file: " + filename);
        return block;
    } else {
        utilities::log_error("Failed to deserialize block from file: " + filename);
        return std::nullopt;
    }
}

#endif // FILE_STORAGE_TPP
