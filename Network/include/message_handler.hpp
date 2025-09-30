#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <cstdint>

namespace RType::Network {

    class Session;

    /**
     * @brief Base interface for message handlers
     *
     * Message handlers process specific types of incoming messages
     * and implement the game logic for each message type.
     */
    class IMessageHandler {
    public:
        virtual ~IMessageHandler() = default;

        /**
         * @brief Handle an incoming message
         * @param session Client session that sent the message
         * @param data Message data
         * @param size Size of message data
         * @return true if message was handled successfully
         */
        virtual bool handle_message(std::shared_ptr<Session> session, const char* data, size_t size) = 0;

        /**
         * @brief Get the message type this handler processes
         */
        virtual uint8_t get_message_type() const = 0;
    };

    /**
     * @brief Message dispatcher that routes messages to appropriate handlers
     *
     * This class manages message handlers and routes incoming messages
     * based on message type to the correct handler.
     */
    class MessageDispatcher {
    public:
        using handler_ptr = std::shared_ptr<IMessageHandler>;

        /**
         * @brief Register a message handler for a specific message type
         * @param message_type Message type to handle
         * @param handler Handler instance
         */
        void register_handler(uint8_t message_type, handler_ptr handler);

        /**
         * @brief Unregister a message handler
         * @param message_type Message type to unregister
         */
        void unregister_handler(uint8_t message_type);

        /**
         * @brief Dispatch a message to the appropriate handler
         * @param session Client session that sent the message
         * @param data Message data
         * @param size Size of message data
         * @return true if message was handled successfully
         */
        bool dispatch_message(std::shared_ptr<Session> session, const char* data, size_t size);

        /**
         * @brief Check if a handler is registered for a message type
         */
        bool has_handler(uint8_t message_type) const;

    private:
        std::unordered_map<uint8_t, handler_ptr> handlers_;
    };

    /**
     * @brief Template base class for typed message handlers
     *
     * This class provides a convenient base for handlers that work
     * with specific message structures.
     */
    template<typename MessageType>
    class TypedMessageHandler : public IMessageHandler {
    public:
        bool handle_message(std::shared_ptr<Session> session, const char* data, size_t size) override {
            if (size < sizeof(MessageType)) {
                return false; // Message too small
            }

            const MessageType* message = reinterpret_cast<const MessageType*>(data);
            return handle_typed_message(session, *message, data + sizeof(MessageType), size - sizeof(MessageType));
        }

    protected:
        /**
         * @brief Handle a typed message
         * @param session Client session
         * @param message Parsed message structure
         * @param payload Additional payload data
         * @param payload_size Size of payload data
         */
        virtual bool handle_typed_message(
            std::shared_ptr<Session> session,
            const MessageType& message,
            const char* payload,
            size_t payload_size) = 0;
    };

} // namespace RType::Network