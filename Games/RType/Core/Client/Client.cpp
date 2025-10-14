#include "Client.hpp"
#include "../Network/include/protocol.hpp"
#include <iostream>
#include <cstring>

#ifndef _WIN32
  #include <cerrno>
  #include <sys/time.h>
#endif

// --- Helpers spécifiques au timeout recv ---
static bool set_recv_timeout(socket_t s, int seconds) {
#ifdef _WIN32
    int ms = seconds * 1000;
    return setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&ms, sizeof(ms)) == 0;
#else
    timeval tv{};
    tv.tv_sec  = seconds;
    tv.tv_usec = 0;
    return setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) == 0;
#endif
}

UDPClient::UDPClient()
    : socket_fd((socket_t)-1), server_port(0), connected(false), running(false) {
    std::memset(&server_address, 0, sizeof(server_address));
    std::memset(&client_address, 0, sizeof(client_address));
#ifdef _WIN32
    WSADATA wsa;
    int rc = WSAStartup(MAKEWORD(2,2), &wsa);
    if (rc != 0) {
        std::cerr << "WSAStartup failed: " << rc << std::endl;
    }
#endif
}

UDPClient::~UDPClient() {
    disconnect();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool UDPClient::setup_socket() {
    // Create UDP socket
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
#ifdef _WIN32
    if (socket_fd == INVALID_SOCKET) {
        std::cerr << "Error creating client socket (WSA: " << WSAGetLastError() << ")\n";
        return false;
    }
#else
    if (socket_fd < 0) {
        std::cerr << "Error creating client socket\n";
        return false;
    }
#endif

    // Bind to any available local port
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = INADDR_ANY;
    client_address.sin_port = 0; // Let system assign port
    if (bind(socket_fd, (struct sockaddr*)&client_address, sizeof(client_address)) < 0) {
        std::cerr << "Error binding client socket\n";
        CLOSESOCKET(socket_fd);
        socket_fd = (socket_t)-1;
        return false;
    }

    // Get the assigned local port
    socklen_t addr_len = sizeof(client_address);
    if (getsockname(socket_fd, (struct sockaddr*)&client_address, &addr_len) < 0) {
        std::cerr << "Error getting client socket info\n";
        CLOSESOCKET(socket_fd);
        socket_fd = (socket_t)-1;
        return false;
    }

    // Set receive timeout for non-blocking-like behavior
    if (!set_recv_timeout(socket_fd, 1)) {
#ifdef _WIN32
        std::cerr << "Warning: could not set client socket recv timeout (WSA: "
                  << WSAGetLastError() << ")\n";
#else
        std::cerr << "Warning: could not set client socket recv timeout\n";
#endif
    }
    return true;
}

bool UDPClient::connect_to_server(const std::string& ip, int port) {
    if (connected.load()) {
        std::cout << "Client already connected\n";
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
        CLOSESOCKET(socket_fd);
        socket_fd = (socket_t)-1;
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

    if (socket_fd != (socket_t)-1) {
        CLOSESOCKET(socket_fd);
        socket_fd = (socket_t)-1;
    }

    std::cout << "Client disconnected\n";
}

bool UDPClient::is_connected() const {
    return connected.load();
}

bool UDPClient::is_running() const {
    return running.load();
}

bool UDPClient::send_message(const std::string& message) {
    return send_data(message.c_str(), (int)message.length());
}

bool UDPClient::send_data(const char* data, int length) {
    if (!connected.load()
#ifdef _WIN32
        || socket_fd == INVALID_SOCKET
#else
        || socket_fd < 0
#endif
        ) {
        std::cerr << "Client not connected\n";
        return false;
    }

    int bytes_sent = (int)sendto(
        socket_fd, data, length, 0,
        (struct sockaddr*)&server_address, (int)sizeof(server_address));

#ifdef _WIN32
    if (bytes_sent == SOCKET_ERROR) {
        std::cerr << "Error sending data to server (WSA: " << WSAGetLastError() << ")\n";
        return false;
    }
#else
    if (bytes_sent < 0) {
        std::cerr << "Error sending data to server\n";
        return false;
    }
#endif

    if (bytes_sent != length) {
        std::cerr << "Warning: only sent " << bytes_sent << " of " << length << " bytes\n";
    }

    return true;
}

void UDPClient::start_receiving() {
    if (running.load()) {
        std::cout << "Receive thread already running\n";
        return;
    }

    if (!connected.load()) {
        std::cerr << "Cannot start receiving: client not connected\n";
        return;
    }

    running.store(true);
    receive_thread = std::thread(&UDPClient::receive_loop, this);
    std::cout << "Started receiving messages from server\n";
}

void UDPClient::stop_receiving() {
    if (running.load()) {
        running.store(false);

        if (receive_thread.joinable()) {
            receive_thread.join();
        }

        std::cout << "Stopped receiving messages\n";
    }
}

void UDPClient::receive_loop() {
    char buffer[1024];
    sockaddr_in from_address;
#ifdef _WIN32
    int from_len = (int)sizeof(from_address);
#else
    socklen_t from_len = sizeof(from_address);
#endif

    std::cout << "Client receive loop started\n";

    while (running.load() && connected.load()) {
        std::memset(buffer, 0, sizeof(buffer));
        std::memset(&from_address, 0, sizeof(from_address));

        int bytes_received = (int)recvfrom(
            socket_fd, buffer, (int)sizeof(buffer) - 1, 0,
            (struct sockaddr*)&from_address, &from_len);

#ifdef _WIN32
        if (bytes_received == SOCKET_ERROR) {
            int err = WSAGetLastError();
            // timeout / would block
            if (err == WSAETIMEDOUT || err == WSAEWOULDBLOCK || err == WSAEINTR) {
                continue;
            }
            std::cerr << "Client recvfrom error (WSA): " << err << "\n";
            break;
        }
#else
        if (bytes_received < 0) {
            // Handle timeout or interrupt
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                continue;
            }
            std::cerr << "Client recvfrom error: " << strerror(errno) << std::endl;
            break;
        }
#endif

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

    std::cout << "Client receive loop ended\n";
}

void UDPClient::print_client_info() const {
    std::cout << "=== UDP Client Info ===\n";
    std::cout << "Server: " << server_ip << ":" << server_port << "\n";
    std::cout << "Local port: " << ntohs(client_address.sin_port) << "\n";
    std::cout << "Connected: " << (connected.load() ? "Yes" : "No") << "\n";
    std::cout << "Receiving: " << (running.load() ? "Yes" : "No") << "\n";
    std::cout << "======================\n";
}
