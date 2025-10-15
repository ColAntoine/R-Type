#pragma once

#include <string>
#include <atomic>
#include <thread>
#include <functional>

#ifdef _WIN32
  #define NOMINMAX
  #define WIN32_LEAN_AND_MEAN
  #include <winsock2.h>
  #include <ws2tcpip.h>
  using socket_t = SOCKET;
  #define CLOSESOCKET(s) closesocket(s)
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  using socket_t = int;
  #define CLOSESOCKET(s) close(s)
#endif


class UDPClient {
    private:
        int socket_fd;
        sockaddr_in server_address;
        sockaddr_in client_address;
        std::string server_ip;
        int server_port;
        std::atomic<bool> connected;
        std::atomic<bool> running;
        std::thread receive_thread;

        void receive_loop();
        bool setup_socket();

    public:
        UDPClient();
        ~UDPClient();

        bool connect_to_server(const std::string& ip, int port);
        void disconnect();
        bool is_connected() const;
        bool is_running() const;

        bool send_message(const std::string& message);
        bool send_data(const char* data, int length);

        void start_receiving();
        void stop_receiving();

        // Callback for received messages (optional)
        std::function<void(const std::string&)> on_message_received;

        void print_client_info() const;
};