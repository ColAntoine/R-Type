#pragma once

#include "message_handler.hpp"
#include "protocol.hpp"

namespace RType::Network {

/**
 * @brief Handler for client connection messages
 */
class ConnectionHandler : public TypedMessageHandler<RType::Protocol::ClientConnect> {
    public:
        explicit ConnectionHandler(std::shared_ptr<class UdpServer> server) : server_(server) {}

        uint8_t get_message_type() const override {
            return static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_CONNECT);
        }

    protected:
        bool handle_typed_message(
            std::shared_ptr<Session> session,
            const RType::Protocol::ClientConnect& message,
            const char* payload,
            size_t payload_size) override;

    private:
        std::shared_ptr<class UdpServer> server_;
};

/**
 * @brief Handler for client ready messages
 */
class ClientReadyHandler : public TypedMessageHandler<RType::Protocol::ClientReady> {
    public:
        explicit ClientReadyHandler(std::shared_ptr<class UdpServer> server) : server_(server) {}

        uint8_t get_message_type() const override {
            return static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_READY);
        }

    protected:
        bool handle_typed_message(
            std::shared_ptr<Session> session,
            const RType::Protocol::ClientReady& message,
            const char* payload,
            size_t payload_size) override;

    private:
        std::shared_ptr<class UdpServer> server_;
};

/**
 * @brief Handler for client disconnect messages
 */
class ClientDisconnectHandler : public TypedMessageHandler<RType::Protocol::ClientDisconnect> {
    public:
        explicit ClientDisconnectHandler(std::shared_ptr<class UdpServer> server) : server_(server) {}

        uint8_t get_message_type() const override {
            return static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_DISCONNECT);
        }

    protected:
        bool handle_typed_message(
            std::shared_ptr<Session> session,
            const RType::Protocol::ClientDisconnect& message,
            const char* payload,
            size_t payload_size) override;

    private:
        std::shared_ptr<class UdpServer> server_;
};

/**
 * @brief Handler for position update messages
 */
class PositionUpdateHandler : public TypedMessageHandler<RType::Protocol::PositionUpdate> {
    public:
        explicit PositionUpdateHandler(std::shared_ptr<class UdpServer> server) : server_(server) {}

        uint8_t get_message_type() const override {
            return static_cast<uint8_t>(RType::Protocol::GameMessage::POSITION_UPDATE);
        }

    protected:
        bool handle_typed_message(
            std::shared_ptr<Session> session,
            const RType::Protocol::PositionUpdate& message,
            const char* payload,
            size_t payload_size) override;

    private:
        std::shared_ptr<class UdpServer> server_;
};

/**
 * @brief Handler for ping messages
 */
class PingHandler : public TypedMessageHandler<RType::Protocol::Ping> {
    public:
        uint8_t get_message_type() const override {
            return static_cast<uint8_t>(RType::Protocol::SystemMessage::PING);
        }

    protected:
        bool handle_typed_message(
            std::shared_ptr<Session> session,
            const RType::Protocol::Ping& message,
            const char* payload,
            size_t payload_size) override;
};

/**
 * @brief Handler for player shooting messages
 */
class PlayerShootHandler : public TypedMessageHandler<RType::Protocol::PlayerShoot> {
    public:
        explicit PlayerShootHandler(std::shared_ptr<class UdpServer> server) : server_(server) {}

        uint8_t get_message_type() const override {
            return static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_SHOOT);
        }

    protected:
        bool handle_typed_message(
            std::shared_ptr<Session> session,
            const RType::Protocol::PlayerShoot& message,
            const char* payload,
            size_t payload_size) override;

    private:
        std::shared_ptr<class UdpServer> server_;
};

} // namespace RType::Network