#include "server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cerrno>
#include <sys/time.h>

UDPServer::UDPServer() : socket_fd(-1), port(0), running(false) {
    memset(&server_address, 0, sizeof(server_address));
}

UDPServer::~UDPServer() {
    stop();
}

bool UDPServer::initialize(int server_port) {
    int opt = 1;
    port = server_port;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return false;
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Error setting socket options" << std::endl;
        close(socket_fd);
        socket_fd = -1;
        return false;
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);
    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Error binding socket to port " << port << std::endl;
        close(socket_fd);
        socket_fd = -1;
        return false;
    }
    // Set receive timeout so recvfrom doesn't block indefinitely and allows clean shutdown
    struct timeval tv;
    tv.tv_sec = 1; // 1 second timeout
    tv.tv_usec = 0;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
        std::cerr << "Warning: could not set socket recv timeout" << std::endl;
        // Not fatal
    }
    std::cout << "UDP Server initialized on port " << port << std::endl;
    return true;
}

bool UDPServer::start() {
    if (socket_fd < 0) {
        std::cerr << "Server not initialized. Call initialize() first." << std::endl;
        return false;
    }

    if (running.load()) {
        std::cout << "Server is already running" << std::endl;
        return true;
    }

    running.store(true);
    server_thread = std::thread(&UDPServer::server_loop, this);

    std::cout << "UDP Server started and listening on port " << port << std::endl;
    return true;
}

void UDPServer::stop() {
    if (running.load()) {
        running.store(false);
        if (server_thread.joinable())
            server_thread.join();
        std::cout << "Server stopped" << std::endl;
    }
    if (socket_fd >= 0) {
        close(socket_fd);
        socket_fd = -1;
    }
    clients.clear();
}

bool UDPServer::is_running() const {
    return running.load();
}

void UDPServer::server_loop() {
    char buffer[1024];
    sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    std::cout << "Server loop started" << std::endl;

    while (running.load()) {
        memset(buffer, 0, sizeof(buffer));
        memset(&client_address, 0, sizeof(client_address));

        ssize_t bytes_received = recvfrom(socket_fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_address, &client_len);
        if (bytes_received < 0) {
            // timeout or interrupted syscall - check running flag and continue
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                continue;
            }
            std::cerr << "recvfrom error: " << strerror(errno) << std::endl;
            break;
        }

        if (bytes_received == 0) {
            continue;
        }
        buffer[bytes_received] = '\0';
        std::string client_key = get_client_key(client_address);
        if (clients.find(client_key) == clients.end()) {
            add_client(client_address);
        }
        clients[client_key].last_ping = time(nullptr);
        handle_client_message(buffer, bytes_received, client_address);
    }

    std::cout << "Server loop ended" << std::endl;
}

std::string UDPServer::get_client_key(const sockaddr_in& client_addr) {
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
    return std::string(ip_str) + ":" + std::to_string(ntohs(client_addr.sin_port));
}

void UDPServer::handle_client_message(const char* buffer, int length, const sockaddr_in& client_addr) {
    std::string client_key = get_client_key(client_addr);
    std::string message(buffer, length);

    std::cout << "Received from " << client_key << ": " << message << std::endl;

    // Echo the message back to the client
    std::string response = "Echo: " + message;
    send_to_client(client_key, response.c_str(), response.length());

    // If client sends "quit", remove them
    if (message == "quit") {
        remove_client(client_key);
    }
}

void UDPServer::add_client(const sockaddr_in& client_addr) {
    std::string client_key = get_client_key(client_addr);

    ClientInfo client_info;
    client_info.address = client_addr;

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
    client_info.ip = std::string(ip_str);
    client_info.port = ntohs(client_addr.sin_port);
    client_info.connected = true;
    client_info.last_ping = time(nullptr);

    clients[client_key] = client_info;

    std::cout << "New client connected: " << client_key << std::endl;
    std::cout << "Total clients: " << clients.size() << std::endl;

    // Send welcome message
    std::string welcome = "Welcome to R-Type server!";
    send_to_client(client_key, welcome.c_str(), welcome.length());
}

void UDPServer::remove_client(const std::string& client_key) {
    auto it = clients.find(client_key);
    if (it != clients.end()) {
        clients.erase(it);
        std::cout << "Client disconnected: " << client_key << std::endl;
        std::cout << "Total clients: " << clients.size() << std::endl;
    }
}

void UDPServer::send_to_client(const std::string& client_key, const char* data, int length) {
    auto it = clients.find(client_key);
    if (it != clients.end()) {
        ssize_t bytes_sent = sendto(socket_fd, data, length, 0,
                                  (struct sockaddr*)&(it->second.address),
                                  sizeof(it->second.address));

        if (bytes_sent < 0) {
            std::cerr << "Error sending data to client " << client_key << std::endl;
        }
    }
}

void UDPServer::send_to_all_clients(const char* data, int length) {
    for (const auto& client_pair : clients) {
        send_to_client(client_pair.first, data, length);
    }
}

void UDPServer::broadcast_message(const std::string& message) {
    std::string broadcast_msg = "Broadcast: " + message;
    send_to_all_clients(broadcast_msg.c_str(), broadcast_msg.length());
    std::cout << "Broadcasted message to " << clients.size() << " clients" << std::endl;
}

int UDPServer::get_client_count() const {
    return clients.size();
}

std::vector<std::string> UDPServer::get_connected_clients() const {
    std::vector<std::string> client_list;
    for (const auto& client_pair : clients) {
        client_list.push_back(client_pair.first);
    }
    return client_list;
}

void UDPServer::print_server_info() const {
    std::cout << "=== UDP Server Info ===" << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "Running: " << (running.load() ? "Yes" : "No") << std::endl;
    std::cout << "Connected clients: " << clients.size() << std::endl;

    if (!clients.empty()) {
        std::cout << "Client list:" << std::endl;
        for (const auto& client_pair : clients) {
            const ClientInfo& info = client_pair.second;
            std::cout << "  - " << client_pair.first
                      << " (last ping: " << (time(nullptr) - info.last_ping) << "s ago)" << std::endl;
        }
    }
    std::cout << "======================" << std::endl;
}