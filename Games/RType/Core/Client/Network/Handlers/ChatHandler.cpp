#include "ChatHandler.hpp"
#include <iostream>
#include <cstring>

ChatHandler::ChatHandler() : chat_callback_(nullptr) {}

void ChatHandler::on_server_chat(const char* payload, size_t size) {
    if (size < sizeof(RType::Protocol::ChatMessage)) {
        std::cerr << "[ChatHandler] Invalid chat message size" << std::endl;
        return;
    }

    const auto* chat_msg = reinterpret_cast<const RType::Protocol::ChatMessage*>(payload);
    
    std::string message(chat_msg->message, strnlen(chat_msg->message, sizeof(chat_msg->message)));
    
    // For now, we don't have the player name here - it will be resolved in the Lobby
    ChatEntry entry;
    entry.player_id = chat_msg->player_id;
    entry.player_name = "Player " + std::to_string(chat_msg->player_id);
    entry.message = message;
    
    chat_history_.push_back(entry);
    
    // Keep only the last MAX_HISTORY messages
    if (chat_history_.size() > MAX_HISTORY) {
        chat_history_.pop_front();
    }

    // Invoke callback
    if (chat_callback_) {
        chat_callback_(std::vector<ChatEntry>(chat_history_.begin(), chat_history_.end()));
    }

    std::cout << "[ChatHandler] Received chat from player " << chat_msg->player_id << ": " << message << std::endl;
}

void ChatHandler::set_chat_callback(ChatCallback callback) {
    chat_callback_ = callback;
}

void ChatHandler::add_local_message(uint32_t player_id, const std::string& player_name, const std::string& message) {
    ChatEntry entry;
    entry.player_id = player_id;
    entry.player_name = player_name;
    entry.message = message;
    
    chat_history_.push_back(entry);
    
    // Keep only the last MAX_HISTORY messages
    if (chat_history_.size() > MAX_HISTORY) {
        chat_history_.pop_front();
    }

    // Invoke callback
    if (chat_callback_) {
        chat_callback_(std::vector<ChatEntry>(chat_history_.begin(), chat_history_.end()));
    }
}

void ChatHandler::clear_history() {
    chat_history_.clear();
}
