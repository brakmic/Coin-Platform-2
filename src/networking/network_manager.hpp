#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <deque>
#include <asio.hpp>
#include "../common/types.hpp"
#include "../common/utilities.hpp"

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

    // Starts the networking services
    bool start(Port port);

    // Stops the networking services
    void stop();

    // Connects to a peer
    bool connect_to_peer(const IPAddress& ip, Port port);

    // Sends data to a connected peer
    bool send_data(const IPAddress& ip, const bytes& data);

    // Broadcasts data to all connected peers
    void broadcast_data(const bytes& data);

    // Sets the callback function for received data
    void set_receive_callback(std::function<void(const IPAddress&, const bytes&)> callback);

private:
    // Peer session
    struct PeerSession : public std::enable_shared_from_this<PeerSession> {
        asio::ip::tcp::socket socket;
        IPAddress ip;
        std::deque<bytes> write_queue;

        PeerSession(asio::ip::tcp::socket socket);
        void start(std::function<void(const IPAddress&, const bytes&)> receive_callback);
        void read();
        void write();
        void do_write(bytes data);

    private:
        std::function<void(const IPAddress&, const bytes&)> read_callback;
    };

    asio::io_context io_context_;
    std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
    std::unordered_map<IPAddress, std::shared_ptr<PeerSession>> peers_;
    std::function<void(const IPAddress&, const bytes&)> receive_callback_;
    std::thread io_thread_;
    std::mutex peers_mutex_;
    std::atomic<bool> running_;

    // Internal methods
    void do_accept();
};

#endif // NETWORK_MANAGER_HPP
