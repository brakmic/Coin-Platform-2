#include "config/config.hpp"
#include "node/node.hpp"
#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

std::atomic<bool> running(true);

void signal_handler(int signal)
{
  running = false;
}

int main(int argc, char *argv[])
{
  // Load configuration
  Config &config = Config::get_instance();
  if (!config.load_from_command_line(argc, argv))
  {
    std::cerr << "Failed to load configuration from command line." << std::endl;
    return 1;
  }

  // Create and start the node
  Node node(config);

  // Add known peers (for testing, you can hardcode or read from config)
  if (config.port == 8001)
  {
    node.add_known_peer("127.0.0.1", 8002);
  }
  else if (config.port == 8002)
  {
    node.add_known_peer("127.0.0.1", 8001);
  }

  if (!node.start())
  {
    std::cerr << "Failed to start the node." << std::endl;
    return 1;
  }

  // Set up signal handler for graceful shutdown
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  // Run until interrupted
  std::cout << "Node is running. Press Ctrl+C to stop." << std::endl;
  while (running)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  // Stop the node
  node.stop();

  return 0;
}
