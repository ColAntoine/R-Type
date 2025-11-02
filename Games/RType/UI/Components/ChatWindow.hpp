/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Chat Window Component for lobby
*/

#pragma once

#include "ECS/UI/Components/Panel.hpp"
#include "ECS/UI/Components/Text.hpp"
#include "ECS/UI/Components/InputField.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include <string>
#include <vector>
#include <functional>

namespace RType {

    /**
     * @brief Entry for chat messages display
     */
    struct ChatMessage {
        std::string player_name;
        std::string message;
    };

    /**
     * @brief Chat window component for displaying and managing chat
     */
    class ChatWindow : public UI::UIPanel {
    public:
        using OnMessageSent = std::function<void(const std::string&)>;

        ChatWindow(float x, float y, float width, float height)
            : UIPanel(x, y, width, height) {
            _messages.clear();
        }

        // Add a message to chat display
        void add_message(const std::string& player_name, const std::string& message) {
            _messages.push_back({player_name, message});
            
            // Keep only last 20 messages visible
            if (_messages.size() > 20) {
                _messages.erase(_messages.begin());
            }
        }

        // Clear chat
        void clear_messages() {
            _messages.clear();
        }

        // Get all messages
        const std::vector<ChatMessage>& get_messages() const {
            return _messages;
        }

        // Set callback for when user sends a message
        void set_on_message_sent(OnMessageSent callback) {
            _on_message_sent = callback;
        }

        // Submit current input as a message
        void submit_message(const std::string& message) {
            if (!message.empty() && _on_message_sent) {
                _on_message_sent(message);
            }
        }

    private:
        std::vector<ChatMessage> _messages;
        OnMessageSent _on_message_sent;
    };
}
