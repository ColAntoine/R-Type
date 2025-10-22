#include "UDPClient.hpp"
#include <iostream>
#include <chrono>


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
        socket_.open(asio::ip::udp::v4());
        socket_.set_option(asio::socket_base::broadcast(false));
        socket_.set_option(asio::socket_base::reuse_address(true));

        // Build connect packet
        RType::Protocol::PacketBuilder builder;
        builder.begin_packet(static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_CONNECT));
        RType::Protocol::ClientConnect cc{};
        strncpy(cc.player_name, player_name.c_str(), sizeof(cc.player_name)-1);
        cc.client_version = client_version;
        builder.add_struct(cc);
        auto buf = builder.finalize();

        socket_.send_to(asio::buffer(buf), server_endpoint_);

        // Wait for response with timeout
        socket_.non_blocking(true);
        auto start = std::chrono::steady_clock::now();
        std::vector<uint8_t> recvbuf(1024);
        while (true) {
            asio::error_code ec;
            size_t len = socket_.receive_from(asio::buffer(recvbuf), server_endpoint_, 0, ec);
            if (!ec && len >= RType::Protocol::HEADER_SIZE) {
                // parse header
                RType::Protocol::PacketHeader hdr;
                memcpy(&hdr, recvbuf.data(), sizeof(hdr));
                if (hdr.message_type == static_cast<uint8_t>(RType::Protocol::SystemMessage::SERVER_ACCEPT)) {
                    if (hdr.payload_size >= sizeof(RType::Protocol::ServerAccept)) {
                        RType::Protocol::ServerAccept sa;
                        memcpy(&sa, recvbuf.data() + RType::Protocol::HEADER_SIZE, sizeof(sa));
                        return sa;
                    }
                }
            }
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > timeout_ms) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } catch (const std::exception &e) {
        std::cerr << "UDPClient connect error: " << e.what() << std::endl;
    }
    return std::nullopt;
}

void UdpClient::disconnect() {
    stop_receiving();
    asio::error_code ec;
    if (socket_.is_open()) socket_.close(ec);
}

void UdpClient::send(const void* data, size_t size) {
    if (!socket_.is_open()) {
        std::cerr << "UDPClient::send error: socket not open" << std::endl;
        return;
    }
    
    try {
        socket_.send_to(asio::buffer(data, size), server_endpoint_);
    } catch (const std::exception& e) {
        std::cerr << "UDPClient::send error: " << e.what() << std::endl;
    }
}

void UdpClient::start_receiving(RType::Network::MessageQueue* message_queue) {
    if (receiving_ || !socket_.is_open()) return;

    receiving_ = true;
    receive_thread_ = std::thread(&UdpClient::receive_loop, this, message_queue);
}

void UdpClient::stop_receiving() {
    receiving_ = false;
    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }
}

void UdpClient::receive_loop(RType::Network::MessageQueue* message_queue) {
    std::vector<uint8_t> recvbuf(1024);
    
    while (receiving_ && socket_.is_open()) {
        try {
            asio::error_code ec;
            asio::ip::udp::endpoint sender_endpoint;
            size_t len = socket_.receive_from(asio::buffer(recvbuf), sender_endpoint, 0, ec);
            
            if (!ec && len >= RType::Protocol::HEADER_SIZE) {
                // Parse header
                RType::Protocol::PacketHeader hdr;
                memcpy(&hdr, recvbuf.data(), sizeof(hdr));
                
                // Build dispatcher_data: message_type + payload
                std::vector<char> dispatcher_data;
                dispatcher_data.reserve(1 + hdr.payload_size);
                dispatcher_data.push_back(hdr.message_type);
                dispatcher_data.insert(dispatcher_data.end(), 
                    recvbuf.data() + RType::Protocol::HEADER_SIZE, 
                    recvbuf.data() + RType::Protocol::HEADER_SIZE + hdr.payload_size);
                
                // Queue the message
                RType::Network::ReceivedPacket pkt;
                pkt.session_id = sender_endpoint.address().to_string() + ":" + std::to_string(sender_endpoint.port());
                pkt.data = std::move(dispatcher_data);
                message_queue->push(std::move(pkt));
            }
        } catch (const std::exception& e) {
            if (receiving_) {
                std::cerr << "UDPClient receive error: " << e.what() << std::endl;
            }
        }
    }
}
