#include "Client.hpp"
#include "protocol.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include <sys/time.h>

UDPClient::UDPClient() : socket_fd(-1), server_port(0), connected(false), running(false) {
    memset(&server_address, 0, sizeof(server_address));
    memset(&client_address, 0, sizeof(client_address));
}

UDPClient::~UDPClient() {
    disconnect();
}

bool UDPClient::setup_socket() {
    // Create UDP socket
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        std::cerr << "Error creating client socket" << std::endl;
        return false;
    }

    // Bind to any available local port
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = INADDR_ANY;
    client_address.sin_port = 0; // Let system assign port
    if (bind(socket_fd, (struct sockaddr*)&client_address, sizeof(client_address)) < 0) {
        std::cerr << "Error binding client socket" << std::endl;
        close(socket_fd);
        socket_fd = -1;
        return false;
    }

    // Get the assigned local port
    socklen_t addr_len = sizeof(client_address);
    if (getsockname(socket_fd, (struct sockaddr*)&client_address, &addr_len) < 0) {
        std::cerr << "Error getting client socket info" << std::endl;
        close(socket_fd);
        socket_fd = -1;
        return false;
    }

    // Set receive timeout for non-blocking behavior
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
        std::cerr << "Warning: could not set client socket recv timeout" << std::endl;
    }
    return true;
}

bool UDPClient::connect_to_server(const std::string& ip, int port) {
    if (connected.load()) {
        std::cout << "Client already connected" << std::endl;
        return true;
    }

    server_ip = ip;
    server_port = port;

    if (!setup_socket())
        return false;

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip.c_str(), &server_address.sin_addr) <= 0) {
        std::cerr << "Invalid server IP address: " << server_ip << std::endl;
        close(socket_fd);
        socket_fd = -1;
        return false;
    }

    connected.store(true);

    std::cout << "Client connected to " << server_ip << ":" << server_port
              << " from local port " << ntohs(client_address.sin_port) << std::endl;

    // Send initial connection message
    send_message("CLIENT_CONNECT");

    return true;
}

void UDPClient::disconnect() {
    if (connected.load()) {
        connected.store(false);
    }

    stop_receiving();

    if (socket_fd >= 0) {
        close(socket_fd);
        socket_fd = -1;
    }

    std::cout << "Client disconnected" << std::endl;
}

bool UDPClient::is_connected() const {
    return connected.load();
}

bool UDPClient::is_running() const {
    return running.load();
}

bool UDPClient::send_message(const std::string& message) {
    return send_data(message.c_str(), message.length());
}

bool UDPClient::send_data(const char* data, int length) {
    if (!connected.load() || socket_fd < 0) {
        std::cerr << "Client not connected" << std::endl;
        return false;
    }

    ssize_t bytes_sent = sendto(socket_fd, data, length, 0, (struct sockaddr*)&server_address, sizeof(server_address));

    if (bytes_sent < 0) {
        std::cerr << "Error sending data to server" << std::endl;
        return false;
    }

    if (bytes_sent != length) {
        std::cerr << "Warning: only sent " << bytes_sent << " of " << length << " bytes" << std::endl;
    }

    return true;
}

void UDPClient::start_receiving() {
    if (running.load()) {
        std::cout << "Receive thread already running" << std::endl;
        return;
    }

    if (!connected.load()) {
        std::cerr << "Cannot start receiving: client not connected" << std::endl;
        return;
    }

    running.store(true);
    receive_thread = std::thread(&UDPClient::receive_loop, this);
    std::cout << "Started receiving messages from server" << std::endl;
}

void UDPClient::stop_receiving() {
    if (running.load()) {
        running.store(false);

        if (receive_thread.joinable()) {
            receive_thread.join();
        }

        std::cout << "Stopped receiving messages" << std::endl;
    }
}

void UDPClient::receive_loop() {
    char buffer[1024];
    sockaddr_in from_address;
    socklen_t from_len = sizeof(from_address);

    std::cout << "Client receive loop started" << std::endl;

    while (running.load() && connected.load()) {
        memset(buffer, 0, sizeof(buffer));
        memset(&from_address, 0, sizeof(from_address));

        ssize_t bytes_received = recvfrom(socket_fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&from_address, &from_len);

        if (bytes_received < 0) {
            // Handle timeout or interrupt
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                continue;
            }
            std::cerr << "Client recvfrom error: " << strerror(errno) << std::endl;
            break;
        }

        if (bytes_received == 0) {
            continue;
        }

        buffer[bytes_received] = '\0';
        std::string message(buffer, bytes_received);

        // Call callback if set
        if (on_message_received) {
            on_message_received(message);
        }
    }

    std::cout << "Client receive loop ended" << std::endl;
}

void UDPClient::print_client_info() const {
    std::cout << "=== UDP Client Info ===" << std::endl;
    std::cout << "Server: " << server_ip << ":" << server_port << std::endl;
    std::cout << "Local port: " << ntohs(client_address.sin_port) << std::endl;
    std::cout << "Connected: " << (connected.load() ? "Yes" : "No") << std::endl;
    std::cout << "Receiving: " << (running.load() ? "Yes" : "No") << std::endl;
    std::cout << "======================" << std::endl;
}