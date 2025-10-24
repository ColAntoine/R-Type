#include "NetworkManager.hpp"
#include <iostream>

NetworkManager::NetworkManager(std::shared_ptr<UdpClient> client, registry& registry, DLLoader& loader)
    : client_(client), registry_(registry), loader_(loader), player_handler_(registry, loader), enemy_handler_()
{
}

NetworkManager::~NetworkManager() {
    stop();
}

void NetworkManager::start() {
    if (!client_) return;
    // start receive loop forwarding every packet to the dispatcher
    using RType::Protocol::GameMessage;
    client_->start_receive_loop([this](uint8_t msg_type, const char* payload, size_t size) {
        // dispatch game messages (game messages are >= ENTITY_CREATE per Protocol)
        if (msg_type >= static_cast<uint8_t>(GameMessage::ENTITY_CREATE)) {
            dispatcher_.dispatch(msg_type, payload, size);
        } else {
            // handle system messages if necessary
        }
    });
}

void NetworkManager::stop() {
    if (client_) client_->stop_receive_loop();
}

void NetworkManager::register_handler(uint8_t msg_type, NetworkDispatcher::Handler h) {
    dispatcher_.register_handler(msg_type, std::move(h));
}

void NetworkManager::register_default_handlers() {
    // player join
    using RType::Protocol::GameMessage;
    dispatcher_.register_handler(static_cast<uint8_t>(GameMessage::PLAYER_JOIN),
        [this](const char* payload, size_t size) {
            // copy payload for main-thread job
            std::vector<char> data(payload, payload + size);
            this->post_to_main([this, data = std::move(data)]() mutable {
                player_handler_.on_player_join(data.data(), data.size());
            });
        }
    );

    // player spawn (own player) - created by server for the owning client
    dispatcher_.register_handler(static_cast<uint8_t>(GameMessage::PLAYER_SPAWN),
        [this](const char* payload, size_t size) {
            std::vector<char> data(payload, payload + size);
            this->post_to_main([this, data = std::move(data)]() mutable {
                player_handler_.on_player_spawn(data.data(), data.size());
            });
        }
    );

    // remote player spawn (other clients' players)
    dispatcher_.register_handler(static_cast<uint8_t>(GameMessage::PLAYER_SPAWN_REMOTE),
        [this](const char* payload, size_t size) {
            std::vector<char> data(payload, payload + size);
            this->post_to_main([this, data = std::move(data)]() mutable {
                player_handler_.on_player_remote_spawn(data.data(), data.size());
            });
        }
    );

    // enemy spawn (EntityCreate in this protocol)
    dispatcher_.register_handler(static_cast<uint8_t>(GameMessage::ENTITY_CREATE),
        [this](const char* payload, size_t size) {
            if (!payload || size < sizeof(RType::Protocol::EntityCreate)) return;
            RType::Protocol::EntityCreate ec;
            memcpy(&ec, payload, sizeof(ec));
            uint32_t my_token = 0;
            if (client_) my_token = client_->get_session_token();

            std::vector<char> data(payload, payload + size);
            // If we don't yet have a session token, queue entity creates until handshake completes
            if (client_ && client_->get_session_token() == 0) {
                std::lock_guard lk(pending_entity_mutex_);
                pending_entity_creates_.push_back(std::move(data));
                return;
            }

            if (ec.entity_type == 0 && ec.entity_id == my_token) {
                this->post_to_main([this, data = std::move(data)]() mutable {
                    player_handler_.on_entity_create(data.data(), data.size());
                });
            } else if (ec.entity_type == 0) {
                // other player's spawn -> treat as remote player
                this->post_to_main([this, data = std::move(data)]() mutable {
                    player_handler_.on_player_remote_spawn(data.data(), data.size());
                });
            } else {
                // non-player entity
                this->post_to_main([this, data = std::move(data)]() mutable {
                    enemy_handler_.on_enemy_spawn(data.data(), data.size());
                });
            }
        }
    );

    // player leave
    dispatcher_.register_handler(static_cast<uint8_t>(GameMessage::PLAYER_LEAVE),
        [this](const char* payload, size_t size) {
            std::vector<char> data(payload, payload + size);
            this->post_to_main([this, data = std::move(data)]() mutable {
                player_handler_.on_player_quit(data.data(), data.size());
            });
        }
    );
}

void NetworkManager::post_to_main(std::function<void()> job) {
    std::lock_guard lk(pending_mutex_);
    pending_jobs_.push_back(std::move(job));
}

void NetworkManager::process_pending() {
    std::deque<std::function<void()>> local;
    {
        std::lock_guard lk(pending_mutex_);
        local.swap(pending_jobs_);
    }
    for (auto &j : local) {
        try { j(); } catch (...) {}
    }

    // If we have acquired a session token during this tick, process any queued ENTITY_CREATE messages
    if (client_ && client_->get_session_token() != 0) {
        std::vector<std::vector<char>> pending;
        {
            std::lock_guard lk(pending_entity_mutex_);
            pending.swap(pending_entity_creates_);
        }
        for (auto &data : pending) {
            if (data.size() < sizeof(RType::Protocol::EntityCreate)) continue;
            RType::Protocol::EntityCreate ec;
            memcpy(&ec, data.data(), sizeof(ec));
            if (ec.entity_type == 0 && ec.entity_id == client_->get_session_token()) {
                try { player_handler_.on_entity_create(data.data(), data.size()); } catch(...){}
            } else if (ec.entity_type == 0) {
                try { player_handler_.on_player_remote_spawn(data.data(), data.size()); } catch(...){}
            } else {
                try { enemy_handler_.on_enemy_spawn(data.data(), data.size()); } catch(...){ }
            }
        }
    }
}
