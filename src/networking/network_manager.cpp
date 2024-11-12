#include "network_manager.hpp"
#include <system_error> // For std::error_code

NetworkManager::NetworkManager()
    : io_context_(),
      running_(false) {
}

NetworkManager::~NetworkManager() {
    stop();
}

bool NetworkManager::start(Port port) {
    try {
        acceptor_ = std::make_unique<asio::ip::tcp::acceptor>(
            io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));

        do_accept();

        running_ = true;
        io_thread_ = std::thread([this]() {
            io_context_.run();
        });

        utilities::log_info("NetworkManager started on port " + std::to_string(port));
        return true;
    } catch (const std::exception& e) {
        utilities::log_error("NetworkManager failed to start: " + std::string(e.what()));
        return false;
    }
}

void NetworkManager::stop() {
    if (running_) {
        running_ = false;
        io_context_.stop();
        if (io_thread_.joinable()) {
            io_thread_.join();
        }
        utilities::log_info("NetworkManager stopped.");
    }
}

void NetworkManager::do_accept() {
    acceptor_->async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                auto peer = std::make_shared<PeerSession>(std::move(socket));
                {
                    std::lock_guard<std::mutex> lock(peers_mutex_);
                    peers_[peer->ip] = peer;
                }
                utilities::log_info("Accepted connection from " + peer->ip);
                peer->start(receive_callback_);
            } else {
                utilities::log_error("Accept error: " + ec.message());
            }
            if (running_) {
                do_accept();
            }
        });
}

bool NetworkManager::connect_to_peer(const IPAddress& ip, Port port) {
    try {
        auto socket = std::make_shared<asio::ip::tcp::socket>(io_context_);
        asio::ip::tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(ip, std::to_string(port));

        asio::async_connect(*socket, endpoints,
            [this, socket, ip](std::error_code ec, asio::ip::tcp::endpoint) {
                if (!ec) {
                    auto peer = std::make_shared<PeerSession>(std::move(*socket));
                    {
                        std::lock_guard<std::mutex> lock(peers_mutex_);
                        peers_[ip] = peer;
                    }
                    utilities::log_info("Connected to peer: " + ip);
                    peer->start(receive_callback_);
                } else {
                    utilities::log_error("Failed to connect to peer: " + ip + " - " + ec.message());
                }
            });

        return true;
    } catch (const std::exception& e) {
        utilities::log_error("Exception in connect_to_peer: " + std::string(e.what()));
        return false;
    }
}

bool NetworkManager::send_data(const IPAddress& ip, const bytes& data) {
    std::lock_guard<std::mutex> lock(peers_mutex_);
    auto it = peers_.find(ip);
    if (it != peers_.end()) {
        it->second->do_write(data);
        return true;
    } else {
        utilities::log_error("Peer not found: " + ip);
        return false;
    }
}

void NetworkManager::broadcast_data(const bytes& data) {
    std::lock_guard<std::mutex> lock(peers_mutex_);
    for (auto& [ip, peer] : peers_) {
        peer->do_write(data);
    }
}

void NetworkManager::set_receive_callback(std::function<void(const IPAddress&, const bytes&)> callback) {
    receive_callback_ = callback;
}

NetworkManager::PeerSession::PeerSession(asio::ip::tcp::socket socket)
    : socket(std::move(socket)) {
    ip = this->socket.remote_endpoint().address().to_string();
}

void NetworkManager::PeerSession::start(std::function<void(const IPAddress&, const bytes&)> receive_callback) {
    read_callback = receive_callback;
    read();
}

void NetworkManager::PeerSession::read() {
    auto self(shared_from_this());
    auto buffer = std::make_shared<std::vector<uint8_t>>(1024);
    socket.async_read_some(asio::buffer(*buffer),
        [this, self, buffer](std::error_code ec, std::size_t length) {
            if (!ec) {
                buffer->resize(length);
                if (read_callback) {
                    read_callback(ip, *buffer);
                }
                read();
            } else {
                utilities::log_error("Read error from " + ip + ": " + ec.message());
                // Handle disconnection
                socket.close();
            }
        });
}

void NetworkManager::PeerSession::do_write(bytes data) {
    auto self(shared_from_this());
    bool write_in_progress = !write_queue.empty();
    write_queue.push_back(std::move(data));
    if (!write_in_progress) {
        write();
    }
}

void NetworkManager::PeerSession::write() {
    auto self(shared_from_this());
    asio::async_write(socket,
        asio::buffer(write_queue.front()),
        [this, self](std::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                write_queue.pop_front();
                if (!write_queue.empty()) {
                    write();
                }
            } else {
                utilities::log_error("Write error to " + ip + ": " + ec.message());
                // Handle disconnection
                socket.close();
            }
        });
}
