#include "UDPClient.hpp"
#include <iostream>
#include <chrono>
#include "Core/Server/Protocol/Protocol.hpp"
#include <atomic>


UdpClient::UdpClient()
    : socket_(io_context_)
{
}

UdpClient::~UdpClient() {
    disconnect();
}

std::optional<RType::Protocol::ServerAccept> UdpClient::connect(const std::string &server_ip, uint16_t server_port, const std::string &player_name, uint32_t client_version, int timeout_ms) {
    try {
        server_endpoint_ = asio::ip::udp::endpoint(asio::ip::make_address(server_ip), server_port);
        if (!socket_.is_open()) {
            socket_.open(asio::ip::udp::v4());
            socket_.set_option(asio::socket_base::broadcast(false));
            socket_.set_option(asio::socket_base::reuse_address(true));
            asio::ip::udp::endpoint local_ep(asio::ip::udp::v4(), 0);
            socket_.bind(local_ep);
        } else {
            // If socket is open but not bound, bind to an ephemeral port so we can receive replies.
            asio::error_code ec_local;
            auto local_ep = socket_.local_endpoint(ec_local);
            if (!ec_local) {
                if (local_ep.port() == 0) {
                    asio::ip::udp::endpoint bind_ep(asio::ip::udp::v4(), 0);
                    socket_.bind(bind_ep, ec_local);
                    if (ec_local) std::cerr << "[UDPClient] failed to bind existing open socket: " << ec_local.message() << std::endl;
                }
            } else {
                // If retrieving local_endpoint failed, try to bind anyway
                asio::ip::udp::endpoint bind_ep(asio::ip::udp::v4(), 0);
                socket_.bind(bind_ep, ec_local);
                if (ec_local) std::cerr << "[UDPClient] failed to bind socket: " << ec_local.message() << std::endl;
            }
        }

        bool was_running = recv_running_.load();
        if (was_running) stop_receive_loop(false);

        RType::Protocol::PacketBuilder builder;
        builder.begin_packet(static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_CONNECT));
        RType::Protocol::ClientConnect cc{};
    strncpy(cc.player_name, player_name.c_str(), sizeof(cc.player_name)-1);
    // remember player name for potential reconnects
    last_player_name_ = player_name;
        cc.client_version = client_version;
        builder.add_struct(cc);
        auto buf = builder.finalize();

        asio::error_code send_ec;
        socket_.send_to(asio::buffer(buf), server_endpoint_, 0, send_ec);
        if (send_ec) {
            std::cerr << "[UDPClient] send_to error: " << send_ec.message() << std::endl;
        }

    // Wait for response with timeout. Use a local 'from' endpoint so we can log sender.
    // Put socket in non-blocking mode while we poll for the SERVER_ACCEPT.
    socket_.non_blocking(true);
        auto start = std::chrono::steady_clock::now();
        auto last_send = start;
        std::vector<uint8_t> recvbuf(1024);
        while (true) {
            // resend connect packet every 250ms in case of packet loss
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_send).count() >= 250) {
                asio::error_code send_ec2;
                socket_.send_to(asio::buffer(buf), server_endpoint_, 0, send_ec2);
                if (send_ec2) std::cerr << "[UDPClient] resend send_to error: " << send_ec2.message() << std::endl;
                last_send = now;
            }

            asio::error_code ec;
            asio::ip::udp::endpoint from;
            size_t len = socket_.receive_from(asio::buffer(recvbuf), from, 0, ec);
            if (ec) {
                if (ec != asio::error::would_block && ec != asio::error::try_again) {
                    std::cerr << "[UDPClient] receive error: " << ec.message() << std::endl;
                }
            } else {
                if (len >= RType::Protocol::HEADER_SIZE) {
                    RType::Protocol::PacketHeader hdr;
                    memcpy(&hdr, recvbuf.data(), sizeof(hdr));
                    std::cerr << "[UDPClient] recv msg_type=" << int(hdr.message_type)
                              << " from " << from.address().to_string() << ":" << from.port()
                              << " payload=" << hdr.payload_size << " len=" << len << std::endl;
                        if (hdr.message_type == static_cast<uint8_t>(RType::Protocol::SystemMessage::SERVER_ACCEPT)) {
                        if (hdr.payload_size >= sizeof(RType::Protocol::ServerAccept)) {
                            RType::Protocol::ServerAccept sa;
                            memcpy(&sa, recvbuf.data() + RType::Protocol::HEADER_SIZE, sizeof(sa));
                            set_session_token(sa.session_id);
                            // If a receive handler is registered, ensure the async receive loop is running
                            if (recv_handler_) start_receive_loop(recv_handler_);
                            return sa;
                        } else {
                            std::cerr << "[UDPClient] received SERVER_ACCEPT with unexpected size" << std::endl;
                        }
                    } else {
                        std::cerr << "[UDPClient] ignoring msg_type=" << int(hdr.message_type) << " while waiting for accept" << std::endl;
                    }
                } else {
                    std::cerr << "[UDPClient] short packet while waiting for accept len=" << len << std::endl;
                }
            }
            auto now2 = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now2 - start).count() > timeout_ms) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    // If we exit timeout loop without receiving accept, restart receiver if needed
    if (was_running) {
        asio::error_code ec;
        socket_.non_blocking(false, ec);
        if (ec) std::cerr << "[UDPClient] failed to set blocking mode: " << ec.message() << std::endl;
        start_receive_loop(recv_handler_);
    }
    } catch (const std::exception &e) {
        std::cerr << "UDPClient connect error: " << e.what() << std::endl;
    }
    return std::nullopt;
}

std::string UdpClient::get_server_ip() const {
    try {
        return server_endpoint_.address().to_string();
    } catch (...) {
        return std::string();
    }
}

void UdpClient::disconnect() {
    // Stop receive loop and close socket safely to avoid races with the recv thread
    stop_receive_loop(true);
}

void UdpClient::send_disconnect(uint32_t player_id) {
    try {
        if (!socket_.is_open()) return;
        RType::Protocol::PacketBuilder builder;
        builder.begin_packet(static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_DISCONNECT));
        RType::Protocol::ClientDisconnect cd{};
        cd.player_id = player_id;
        cd.reason = 0; // voluntary
        builder.add_struct(cd);
        auto buf = builder.finalize();
        socket_.send_to(asio::buffer(buf), server_endpoint_);
    } catch (const std::exception &e) {
        std::cerr << "UDPClient send_disconnect error: " << e.what() << std::endl;
    }
}

void UdpClient::send_packet(const char* data, size_t length) {
    try {
        if (!socket_.is_open()) {
            std::cerr << "[UdpClient] Cannot send: socket is not open" << std::endl;
            return;
        }
        socket_.send_to(asio::buffer(data, length), server_endpoint_);
    } catch (const std::exception &e) {
        std::cerr << "[UdpClient] send_packet error: " << e.what() << std::endl;
    }
}

void UdpClient::start_receive_loop(std::function<void(uint8_t,const char*,size_t)> handler) {
    if (recv_running_.load()) return;
    recv_running_.store(true);
    recv_handler_ = handler;

    // Ensure socket is open before configuring non-blocking mode. If it's not open, attempt to open and bind
    asio::error_code ec;
    if (!socket_.is_open()) {
        // Try to open and bind an ephemeral port so the receive loop can operate
        socket_.open(asio::ip::udp::v4(), ec);
        if (ec) {
            std::cerr << "[UDPClient] could not open socket for recv loop: " << ec.message() << std::endl;
        } else {
            socket_.set_option(asio::socket_base::broadcast(false));
            socket_.set_option(asio::socket_base::reuse_address(true));
            asio::ip::udp::endpoint local_ep(asio::ip::udp::v4(), 0);
            socket_.bind(local_ep, ec);
            if (ec) std::cerr << "[UDPClient] failed to bind socket for recv loop: " << ec.message() << std::endl;
        }
    }

    socket_.non_blocking(true, ec);
    if (ec) std::cerr << "[UDPClient] failed to set non-blocking mode for recv loop: " << ec.message() << std::endl;

    recv_thread_ = std::thread([this]() mutable {
        std::vector<char> recvbuf(1024);
        while (recv_running_.load()) {
            try {
                asio::error_code ec_inner;
                asio::ip::udp::endpoint from;
                size_t len = socket_.receive_from(asio::buffer(recvbuf), from, 0, ec_inner);
                if (len >= RType::Protocol::HEADER_SIZE) {
                    RType::Protocol::PacketHeader hdr;
                    memcpy(&hdr, recvbuf.data(), sizeof(hdr));
                    const char* payload = recvbuf.data() + RType::Protocol::HEADER_SIZE;
                    size_t payload_size = std::min<size_t>(hdr.payload_size, len - RType::Protocol::HEADER_SIZE);
                    if (payload_size > 0) {
                        if (recv_handler_) recv_handler_(hdr.message_type, payload, payload_size);
                    } else {
                        if (recv_handler_) recv_handler_(hdr.message_type, nullptr, 0);
                    }
                }
            } catch (const std::exception &e) {
                std::cerr << "UDPClient receive loop exception: " << e.what() << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
}

void UdpClient::stop_receive_loop(bool close_socket) {
    recv_running_.store(false);
    // Optionally close socket to unblock receive
    if (close_socket && socket_.is_open()) {
        asio::error_code ec;
        socket_.close(ec);
    }
    if (recv_thread_.joinable()) recv_thread_.join();
}
