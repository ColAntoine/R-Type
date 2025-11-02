#pragma once

#include <cstddef>
#include <functional>
#include <vector>
#include <deque>
#include <string>
#include "Core/Server/Protocol/Protocol.hpp"

/**
 * @brief Structure to hold a chat message with player name
 */
struct ChatEntry {
    uint32_t player_id;
    std::string player_name;
    std::string message;
};

class ChatHandler {
    public:
        using ChatCallback = std::function<void(const std::vector<ChatEntry>&)>;

        ChatHandler();
        ~ChatHandler() = default;

        // Handle incoming chat messages from server
        void on_server_chat(const char* payload, size_t size);

        // Register a callback for when SERVER_CHAT is received
        void set_chat_callback(ChatCallback callback);

        // Get current chat history
        const std::deque<ChatEntry>& get_chat_history() const { return chat_history_; }

        // Add a local chat message (for display)
        void add_local_message(uint32_t player_id, const std::string& player_name, const std::string& message);

        // Clear chat history
        void clear_history();

    private:
        std::deque<ChatEntry> chat_history_;  // Last 100 messages
        ChatCallback chat_callback_;
        static constexpr size_t MAX_HISTORY = 100;
};
