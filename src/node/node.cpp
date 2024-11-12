#include "node.hpp"
#include "../common/utilities.hpp"
#include "../storage/file_storage.hpp"
#include "../cryptography/cryptography.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>

Node::Node(const Config &config)
    : node_role_(config.node_role),
      port_(config.port),
      config_(config),
      running_(false)
{
    // Generate a new private key for the node
    private_key_ = cryptography::generate_private_key();
    public_key_ = cryptography::derive_public_key(private_key_);

    // Initialize NetworkManager
    network_manager_ = std::make_shared<NetworkManager>();

    // Initialize storage
    if (node_role_ == "time" || node_role_ == "dual")
    {
        time_chain_storage_ = std::make_shared<FileStorage<TimeBlock>>();
    }
    if (node_role_ == "value" || node_role_ == "dual")
    {
        value_chain_storage_ = std::make_shared<FileStorage<ValueBlock>>();
    }
}


void Node::add_known_peer(const IPAddress &ip, Port port)
{
  known_peers_.emplace_back(ip, port);
}

bool Node::start()
{
  utilities::log_info("Starting node...");

  // Initialize components
  if (!initialize_components())
  {
    utilities::log_error("Failed to initialize components.");
    return false;
  }

  // Start network manager
  if (!network_manager_->start(port_))
  {
    utilities::log_error("Failed to start NetworkManager.");
    return false;
  }

  // Set up receive callback
  network_manager_->set_receive_callback(
      [this](const IPAddress &sender, const bytes &data)
      {
        this->handle_incoming_data(sender, data);
      });

  // Connect to known peers with retry logic
  for (const auto &[ip, port] : known_peers_)
  {
    int retries = 5;
    while (retries-- > 0)
    {
      if (network_manager_->connect_to_peer(ip, port))
      {
        utilities::log_info("Connected to peer: " + ip);
        break;
      }
      else
      {
        utilities::log_error("Failed to connect to peer: " + ip + ". Retrying in 2 seconds...");
        std::this_thread::sleep_for(std::chrono::seconds(2));
      }
    }
    if (retries <= 0)
    {
      utilities::log_error("Exhausted all retries to connect to peer: " + ip);
    }
  }

  // Start consensus loops
  running_ = true;
  if (node_role_ == "time" || node_role_ == "dual")
  {
    time_chain_thread_ = std::thread(&Node::run_time_chain_loop, this);
  }
  if (node_role_ == "value" || node_role_ == "dual")
  {
    value_chain_thread_ = std::thread(&Node::run_value_chain_loop, this);
  }

  utilities::log_info("Node started.");
  return true;
}

void Node::stop()
{
  utilities::log_info("Stopping node...");

  running_ = false;

  // Stop consensus loops
  if (time_chain_thread_.joinable())
  {
    time_chain_thread_.join();
  }
  if (value_chain_thread_.joinable())
  {
    value_chain_thread_.join();
  }

  // Stop network manager
  network_manager_->stop();

  utilities::log_info("Node stopped.");
}

bool Node::initialize_components()
{
  // Initialize storage components
  if (node_role_ == "time" || node_role_ == "dual")
  {
    if (!initialize_time_chain())
    {
      utilities::log_error("Failed to initialize Time Chain.");
      return false;
    }
  }
  if (node_role_ == "value" || node_role_ == "dual")
  {
    if (!initialize_value_chain())
    {
      utilities::log_error("Failed to initialize Value Chain.");
      return false;
    }
  }
  return true;
}

bool Node::initialize_time_chain()
{
  // Initialize storage
  if (!time_chain_storage_->initialize("data/time_chain"))
  {
    utilities::log_error("Failed to initialize Time Chain storage.");
    return false;
  }

  // Initialize TimeChain
  time_chain_ = std::make_shared<TimeChain>(time_chain_storage_);

  // Initialize TimeChainConsensus
  time_chain_consensus_ = std::make_unique<TimeChainConsensus>(
      time_chain_storage_, network_manager_, private_key_);

  if (!time_chain_consensus_->initialize())
  {
    utilities::log_error("Failed to initialize Time Chain Consensus.");
    return false;
  }

  return true;
}

bool Node::initialize_value_chain()
{
  // Initialize storage
  if (!value_chain_storage_->initialize("data/value_chain"))
  {
    utilities::log_error("Failed to initialize Value Chain storage.");
    return false;
  }

  // Initialize ValueChain
  value_chain_ = std::make_shared<ValueChain>(value_chain_storage_);

  // Initialize ValueChainConsensus
  value_chain_consensus_ = std::make_unique<ValueChainConsensus>(
      value_chain_storage_, network_manager_, time_chain_, private_key_);

  if (!value_chain_consensus_->initialize())
  {
    utilities::log_error("Failed to initialize Value Chain Consensus.");
    return false;
  }

  return true;
}

void Node::handle_incoming_time_block(const IPAddress &sender, const bytes &data)
{
  // Deserialize TimeBlock
  TimeBlock block;
  if (!block.deserialize(data))
  {
    utilities::log_error("Failed to deserialize TimeBlock from " + sender);
    return;
  }
  utilities::log_info("Successfully deserialized TimeBlock from " + sender + " with Hash: " + utilities::bytes_to_hex(bytes(block.get_hash().begin(), block.get_hash().end())));

  // Handle block
  time_chain_consensus_->handle_block(block);
}

void Node::handle_incoming_value_block(const IPAddress &sender, const bytes &data)
{
  // Deserialize ValueBlock
  ValueBlock block;
  if (!block.deserialize(data))
  {
    utilities::log_error("Failed to deserialize ValueBlock from " + sender);
    return;
  }
  utilities::log_info("Successfully deserialized ValueBlock from " + sender);
  // Handle block
  value_chain_consensus_->handle_block(block);
}

void Node::handle_incoming_transaction(const IPAddress &sender, const bytes &data)
{
  // Check if data size meets the minimum expected size
  const size_t MIN_TRANSACTION_SIZE = 32 + 32 + 8 + 8 + 0 + 64 + 32; // 176 bytes
  if (data.size() < MIN_TRANSACTION_SIZE)
  {
    utilities::log_error("Received Transaction data size too small: " + std::to_string(data.size()) + " bytes from " + sender);
    return;
  }

  // Deserialize Transaction
  Transaction tx;
  if (!tx.deserialize(data))
  {
    utilities::log_error("Failed to deserialize Transaction from " + sender);
    return;
  }

  // Add transaction to the pool
  value_chain_consensus_->add_transaction(tx);
  utilities::log_info("Transaction deserialized and added to the pool from " + sender);
}

void Node::run_time_chain_loop()
{
  utilities::log_info("TimeChain loop started.");
  while (running_)
  {
    // Produce block if eligible
    auto block_opt = time_chain_consensus_->produce_block();
    if (block_opt)
    {
      // Serialize the block
      bytes serialized_block = block_opt->serialize();

      // Prepend the message type and sender's public key
      bytes payload;
      bytes sender_pub_key(public_key_.begin(), public_key_.end());
      payload.insert(payload.end(), sender_pub_key.begin(), sender_pub_key.end());
      payload.insert(payload.end(), serialized_block.begin(), serialized_block.end());

      // Construct the full message with length header
      bytes message;

      // Calculate total message size: Message Type + Payload
      uint32_t message_length = 1 + payload.size(); // 1 byte for message type

      // Convert to big-endian
      uint32_t message_length_be = htonl(message_length);
      bytes length_bytes(reinterpret_cast<const byte *>(&message_length_be), reinterpret_cast<const byte *>(&message_length_be) + sizeof(uint32_t));
      message.insert(message.end(), length_bytes.begin(), length_bytes.end());

      // Append message type
      message.push_back(0x01); // Message type for TimeBlock

      // Append payload
      message.insert(message.end(), payload.begin(), payload.end());

      // Broadcast the block
      network_manager_->broadcast_data(message);
      utilities::log_info("TimeBlock broadcasted.");
    }

    // Sleep for a short duration
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  utilities::log_info("TimeChain loop terminated.");
}

void Node::run_value_chain_loop()
{
  utilities::log_info("ValueChain loop started.");
  while (running_)
  {
    // Generate and broadcast a transaction every few seconds
    generate_and_broadcast_transaction();

    // Produce block if eligible
    auto block_opt = value_chain_consensus_->produce_block();
    if (block_opt)
    {
      // Serialize the block
      bytes serialized_block = block_opt->serialize();

      // Prepend the message type and sender's public key
      bytes payload;
      bytes sender_pub_key(public_key_.begin(), public_key_.end());
      payload.insert(payload.end(), sender_pub_key.begin(), sender_pub_key.end());
      payload.insert(payload.end(), serialized_block.begin(), serialized_block.end());

      // Construct the full message with length header
      bytes message;

      // Calculate total message size: Message Type + Payload
      uint32_t message_length = 1 + payload.size(); // 1 byte for message type

      // Convert to big-endian
      uint32_t message_length_be = htonl(message_length);
      bytes length_bytes(reinterpret_cast<const byte *>(&message_length_be), reinterpret_cast<const byte *>(&message_length_be) + sizeof(uint32_t));
      message.insert(message.end(), length_bytes.begin(), length_bytes.end());

      // Append message type
      message.push_back(0x02); // Message type for ValueBlock

      // Append payload
      message.insert(message.end(), payload.begin(), payload.end());

      // Broadcast the block
      network_manager_->broadcast_data(message);
      utilities::log_info("ValueBlock broadcasted.");
    }

    // Sleep for a short duration
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
  utilities::log_info("ValueChain loop terminated.");
}

void Node::generate_and_broadcast_transaction()
{
  // Create a transaction
  PrivateKey recipient_private_key = cryptography::generate_private_key();
  PublicKey recipient_public_key = cryptography::derive_public_key(recipient_private_key);

  uint64_t amount = 10;
  Transaction tx(public_key_, recipient_public_key, amount);

  // Sign the transaction
  bytes tx_data_to_sign = tx.get_data_to_sign();
  Signature signature = cryptography::sign_message(tx_data_to_sign, private_key_);
  tx.set_signature(signature);

  // Add transaction to own pool
  value_chain_consensus_->add_transaction(tx);

  // Serialize the transaction
  bytes serialized_tx = tx.serialize();

  // Ensure serialized_tx has the correct size
  if (serialized_tx.size() != 176) // 32+32+8+8+0+64+32
  {
    utilities::log_error("Serialized Transaction size mismatch: " + std::to_string(serialized_tx.size()) + " bytes.");
  }

  // Construct the full message with length header
  bytes message;

  // Calculate total message size: Message Type + Payload
  uint32_t message_length = 1 + serialized_tx.size(); // 1 byte for message type

  // Convert message_length to big-endian
  uint32_t message_length_be = htonl(message_length);
  bytes length_bytes(reinterpret_cast<const byte *>(&message_length_be), reinterpret_cast<const byte *>(&message_length_be) + sizeof(uint32_t));
  message.insert(message.end(), length_bytes.begin(), length_bytes.end());

  // Append message type
  message.push_back(0x03); // Message type for Transaction

  // Append payload
  message.insert(message.end(), serialized_tx.begin(), serialized_tx.end());

  // Broadcast the transaction
  network_manager_->broadcast_data(message);
  utilities::log_info("Transaction broadcasted.");
}

void Node::handle_incoming_data(const IPAddress &sender, const bytes &data)
{
  std::lock_guard<std::mutex> lock(buffer_mutex_);
  auto &buffer = incoming_buffers_[sender];
  buffer.insert(buffer.end(), data.begin(), data.end());

  while (buffer.size() >= 4) // At least length header
  {
    // Extract message length (4 bytes, big-endian)
    uint32_t message_length = 0;
    message_length |= static_cast<uint32_t>(buffer[0]) << 24;
    message_length |= static_cast<uint32_t>(buffer[1]) << 16;
    message_length |= static_cast<uint32_t>(buffer[2]) << 8;
    message_length |= static_cast<uint32_t>(buffer[3]);

    // Convert from big-endian to host byte order
    message_length = ntohl(message_length);

    // Check if the entire message has been received
    if (buffer.size() < 4 + message_length)
      break; // Wait for more data

    // Extract the complete message
    bytes complete_message(buffer.begin() + 4, buffer.begin() + 4 + message_length);

    // Remove the processed message from the buffer
    buffer.erase(buffer.begin(), buffer.begin() + 4 + message_length);

    // Process the complete message
    process_complete_message(sender, complete_message);
  }
}

void Node::process_complete_message(const IPAddress &sender, const bytes &message)
{
  if (message.size() < 1)
  { // At least message type
    utilities::log_error("Received message too short from " + sender);
    return;
  }

  byte message_type = message[0];
  bytes payload(message.begin() + 1, message.end());

  if (message_type == 0x01) // TimeBlock
  {
    handle_incoming_time_block(sender, payload);
  }
  else if (message_type == 0x02) // ValueBlock
  {
    handle_incoming_value_block(sender, payload);
  }
  else if (message_type == 0x03) // Transaction
  {
    handle_incoming_transaction(sender, payload);
  }
  else
  {
    utilities::log_error("Unknown message type received from " + sender + ": " + std::to_string(message_type));
  }
}
