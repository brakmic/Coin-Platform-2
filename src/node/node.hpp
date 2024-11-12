#ifndef NODE_HPP
#define NODE_HPP

#include <map>
#include <vector>
#include <string>
#include <mutex>
#include "../config/config.hpp"
#include "../networking/network_manager.hpp"
#include "../storage/storage_interface.hpp"
#include "../cryptography/cryptography.hpp"
#include "../consensus/time_chain_consensus.hpp"
#include "../consensus/value_chain_consensus.hpp"
#include "../time_chain/time_chain.hpp"
#include "../value_chain/value_chain.hpp"
#include <memory>
#include <thread>
#include <atomic>
#include <vector>
#include <utility>

class Node
{
public:
  explicit Node(const Config &config);

  // Starts the node
  bool start();

  // Stops the node
  void stop();

  // Adds a known peer to connect to
  void add_known_peer(const IPAddress &ip, Port port);

private:
  // Node components
  std::shared_ptr<NetworkManager> network_manager_;
  std::shared_ptr<StorageInterface<TimeBlock>> time_chain_storage_;
  std::shared_ptr<StorageInterface<ValueBlock>> value_chain_storage_;
  std::shared_ptr<TimeChain> time_chain_;
  std::shared_ptr<ValueChain> value_chain_;
  std::unique_ptr<TimeChainConsensus> time_chain_consensus_;
  std::unique_ptr<ValueChainConsensus> value_chain_consensus_;
  PrivateKey private_key_;
  PublicKey public_key_;

  // Node configuration
  std::string node_role_;
  Port port_;
  Config config_;

  // Known peers
  std::vector<std::pair<IPAddress, Port>> known_peers_;

  // Initialization methods
  bool initialize_components();
  bool initialize_time_chain();
  bool initialize_value_chain();

  // Event handlers
  void handle_incoming_time_block(const IPAddress &sender, const bytes &data);
  void handle_incoming_value_block(const IPAddress &sender, const bytes &data);
  void handle_incoming_transaction(const IPAddress &sender, const bytes &data);
  void handle_incoming_data(const IPAddress &sender, const bytes &data);

  // Main loops
  void run_time_chain_loop();
  void run_value_chain_loop();

  // Transaction generation
  void generate_and_broadcast_transaction();

  // Thread management
  std::thread time_chain_thread_;
  std::thread value_chain_thread_;
  std::atomic<bool> running_;
  std::map<IPAddress, std::vector<byte>> incoming_buffers_;
  std::mutex buffer_mutex_;

  void process_complete_message(const IPAddress &sender, const bytes &message);
};

#endif // NODE_HPP
