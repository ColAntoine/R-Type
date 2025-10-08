#pragma once

#include <memory>
#include <chrono>
#include <atomic>
#include "connection.hpp"

namespace RType::Network {

    /**
     * @brief Represents a client session with game state
     *
     * This class manages per-client state including player information,
     * connection lifecycle, and session-specific data.
     */
    class Session {
    public:
        using clock_type = std::chrono::steady_clock;

        /**
         * @brief Construct a new Session
         * @param connection Network connection for this session
         */
        explicit Session(std::shared_ptr<Connection> connection);

        /**
         * @brief Get the underlying connection
         */
        std::shared_ptr<Connection> get_connection() const { return connection_; }

        /**
         * @brief Get unique session ID
         */
        const std::string& get_session_id() const { return session_id_; }

        /**
         * @brief Get player ID (assigned when player joins game)
         */
        int get_player_id() const { return player_id_; }

        /**
         * @brief Set player ID
         */
        void set_player_id(int id) { player_id_ = id; }

        /**
         * @brief Check if session is authenticated/active
         */
        bool is_authenticated() const { return authenticated_; }

        /**
         * @brief Set authentication status
         */
        void set_authenticated(bool auth) { authenticated_ = auth; }

        /**
         * @brief Check if client is ready to receive game updates
         */
        bool is_ready() const { return ready_; }

        /**
         * @brief Set client ready status
         */
        void set_ready(bool ready) { ready_ = ready; }

        /**
         * @brief Get player name
         */
        const std::string& get_player_name() const { return player_name_; }

        /**
         * @brief Set player name
         */
        void set_player_name(const std::string& name) { player_name_ = name; }

        /**
         * @brief Get player position
         */
        void get_position(float& x, float& y) const { x = pos_x_; y = pos_y_; }

        /**
         * @brief Set player position
         */
        void set_position(float x, float y) { pos_x_ = x; pos_y_ = y; }

        /**
         * @brief Update last activity timestamp
         */
        void update_activity();

        /**
         * @brief Get time since last activity
         */
        std::chrono::milliseconds get_idle_time() const;

        /**
         * @brief Check if session should be considered timed out
         */
        bool is_timed_out(std::chrono::milliseconds timeout_duration) const;

        /**
         * @brief Send data through this session's connection
         */
        void send(const char* data, size_t size);

        /**
         * @brief Send data async through this session's connection
         */
        template<typename CompletionHandler>
        void async_send(const char* data, size_t size, CompletionHandler&& handler) {
            connection_->async_send(data, size, std::forward<CompletionHandler>(handler));
        }

        /**
         * @brief Disconnect this session
         */
        void disconnect();

        /**
         * @brief Check if session is connected
         */
        bool is_connected() const;

    private:
        std::string generate_session_id();

        std::shared_ptr<Connection> connection_;
        std::string session_id_;
        int player_id_;
        bool authenticated_;
        bool ready_;
        std::string player_name_;

        // Player state
        std::atomic<float> pos_x_{0.0f};
        std::atomic<float> pos_y_{0.0f};

        clock_type::time_point last_activity_;
        static std::atomic<int> next_player_id_;
    };

} // namespace RType::Network