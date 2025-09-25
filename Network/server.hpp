#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct ClientInfo {
    sockaddr_in address;
    std::string ip;
    int port;
    bool connected;
    time_t last_ping;
};

class UDPServer {
    private:
        int socket_fd;
        sockaddr_in server_address;
        int port;
        std::atomic<bool> running;
        std::thread server_thread;
        std::unordered_map<std::string, ClientInfo> clients;

        void server_loop();
        std::string get_client_key(const sockaddr_in& client_addr);
        void handle_client_message(const char* buffer, int length, const sockaddr_in& client_addr);
        void add_client(const sockaddr_in& client_addr);
        void remove_client(const std::string& client_key);

    public:
        UDPServer();
        ~UDPServer();

        bool initialize(int server_port);
        bool start();
        void stop();
        bool is_running() const;

        void send_to_client(const std::string& client_key, const char* data, int length);
        void send_to_all_clients(const char* data, int length);
        void broadcast_message(const std::string& message);

        int get_client_count() const;
        std::vector<std::string> get_connected_clients() const;

        void print_server_info() const;
};