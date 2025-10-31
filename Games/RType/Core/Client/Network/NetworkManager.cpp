#include "NetworkManager.hpp"
#include <iostream>

NetworkManager::NetworkManager(std::shared_ptr<UdpClient> client, registry& registry, ILoader& loader)
    : client_(client), registry_(registry), loader_(loader), player_handler_(registry, loader), enemy_handler_(registry, loader)
{
}

NetworkManager::~NetworkManager() {
    stop();
}

void NetworkManager::start() {
    if (!client_) return;
    // start receive loop forwarding every packet to the dispatcher
    client_->start_receive_loop([this](uint8_t msg_type, const char* payload, size_t size) {
        // dispatch all messages (both system and game messages) to registered handlers
        dispatcher_.dispatch(msg_type, payload, size);
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

    // client list (system message forwarded to main thread)
    using RType::Protocol::SystemMessage;
    dispatcher_.register_handler(static_cast<uint8_t>(SystemMessage::CLIENT_LIST),
        [this](const char* payload, size_t size) {
            std::vector<char> data(payload, payload + size);
            this->post_to_main([this, data = std::move(data)]() mutable {
                player_handler_.on_client_list(data.data(), data.size());
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

    // position update (server broadcasting player positions)
    dispatcher_.register_handler(static_cast<uint8_t>(GameMessage::POSITION_UPDATE),
        [this](const char* payload, size_t size) {
            std::vector<char> data(payload, payload + size);
            this->post_to_main([this, data = std::move(data)]() mutable {
                player_handler_.on_position_update(data.data(), data.size());
            });
        }
    );

    dispatcher_.register_handler(static_cast<uint8_t>(GameMessage::ENTITY_CREATE),
        [this](const char* payload, size_t size) {
            if (!payload || size < sizeof(RType::Protocol::EntityCreate)) return;
            RType::Protocol::EntityCreate ec;
            memcpy(&ec, payload, sizeof(ec));

            std::vector<char> data(payload, payload + size);
            // If we don't yet have a session token, queue entity creates until handshake completes
            if (client_ && client_->get_session_token() == 0) {
                std::lock_guard lk(pending_entity_mutex_);
                pending_entity_creates_.push_back(std::move(data));
                return;
            }

            this->post_to_main([this, data = std::move(data)]() mutable {
                player_handler_.on_entity_create(data.data(), data.size());
            });
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

    // start game
    dispatcher_.register_handler(static_cast<uint8_t>(SystemMessage::START_GAME),
        [this](const char* payload, size_t size) {
            std::vector<char> data(payload, payload + size);
            this->post_to_main([this, data = std::move(data)]() mutable {
                player_handler_.on_game_start(data.data(), data.size());
            });
        }
    );

    // Instance created (server informs client about instance port to reconnect to)
    dispatcher_.register_handler(static_cast<uint8_t>(SystemMessage::INSTANCE_CREATED),
        [this](const char* payload, size_t size) {
            std::vector<char> data(payload, payload + size);
            this->post_to_main([this, data = std::move(data)]() mutable {
                player_handler_.on_instance_created(data.data(), data.size());
            });
        }
    );

    // Instance list (server broadcasts available instances)
    dispatcher_.register_handler(static_cast<uint8_t>(SystemMessage::INSTANCE_LIST),
        [this](const char* payload, size_t size) {
            std::vector<char> data(payload, payload + size);
            this->post_to_main([this, data = std::move(data)]() mutable {
                player_handler_.on_instance_list(data.data(), data.size());
            });
        }
    );

    // game seed (for deterministic gameplay)
    dispatcher_.register_handler(static_cast<uint8_t>(GameMessage::GAME_SEED),
        [this](const char* payload, size_t size) {
            if (!payload || size < sizeof(RType::Protocol::GameSeed)) return;
            
            RType::Protocol::GameSeed seed_msg;
            memcpy(&seed_msg, payload, sizeof(seed_msg));
            
            // Set the seed in the registry on the main thread
            this->post_to_main([this, seed = seed_msg.seed]() {
                registry_.set_random_seed(seed);
                std::cout << "[Client] Received game seed from server: " << seed << std::endl;
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
