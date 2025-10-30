#include "GameServer.hpp"
#include "Network/NetworkManager.hpp"
#include "Network/Session.hpp"
#include "Protocol/MessageQueue.hpp"
#include "ServerECS/ServerECS.hpp"
#include "ServerECS/Communication/Multiplayer.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "Core/Server/States/ServerLobby.hpp"
#include <asio.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <raylib.h>
#include <random>

GameServer::GameServer(bool display, bool windowed, float scale, int maxLobbies, int maxPlayers, bool is_machine_made)
    : port_(8080)
    , running_(false)
    , display_(display)
    , windowed_(windowed)
    , scale_(scale)
    , game_started_(false)
    , max_lobbies_(maxLobbies)
    , max_players_(maxPlayers)
    , is_machine_made_(is_machine_made)
    , zero_clients_timer_active_(false)
    , zero_clients_since_{}
    , zero_clients_grace_(std::chrono::milliseconds(5000))  // 5 seconds grace
{
}

GameServer::~GameServer()
{
    if (running_) {
        shutdown();
    }
}

bool GameServer::init()
{
    std::cout << "[GameServer] Initializing..." << std::endl;

    // Create io_context
    io_context_ = std::make_unique<asio::io_context>();
    network_manager_ = std::make_unique<RType::Network::NetworkManager>();
    if (!network_manager_->initialize(port_)) {
        std::cerr << "[GameServer] Failed to initialize NetworkManager" << std::endl;
        return false;
    }
    message_queue_ = std::make_unique<RType::Network::MessageQueue>();

    // Attach message queue to server
    auto server = network_manager_->get_server();
    if (!server) {
        std::cerr << "[GameServer] No UDP server available" << std::endl;
        return false;
    }
    RType::Network::MessageQueue* msg_ptr = message_queue_.get();
    server->set_message_queue(msg_ptr);

    // Create ServerECS (game logic)
    server_ecs_ = std::make_unique<RType::Network::ServerECS>(max_lobbies_, max_players_);
    server_ecs_->set_message_queue(msg_ptr);
    // Provide a callback so ServerECS can send packets back to specific sessions
    server_ecs_->set_send_callback([server](const std::string& session_id, const std::vector<uint8_t>& packet) {
        if (!server) return;
        server->send_to_client(session_id, reinterpret_cast<const char*>(packet.data()), packet.size());
    });

    // Provide the UDP server pointer to ServerECS/Multiplayer so it can trigger broadcasts directly
    server_ecs_->set_udp_server(server.get());

    // Register instance-request callback so we can spawn instances on demand
    server_ecs_->set_instance_request_callback([this](const std::string &session_id){
        this->handle_instance_request(session_id);
    });

    // Register instance-list request callback so ServerECS can ask the front server
    // to send the current instance list to a newly connected session.
    server_ecs_->set_instance_list_request_callback([this](const std::string &session_id){
        auto server = network_manager_->get_server();
        if (!server) return;
        // Build InstanceList and send to specific client
        RType::Protocol::InstanceList il{};
        il.instance_count = static_cast<uint8_t>(std::min<size_t>(instances_.size(), 8));
        for (size_t i = 0; i < il.instance_count; ++i) {
            il.instances[i] = instances_[i];
        }
        auto packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::SystemMessage::INSTANCE_LIST), il, RType::Protocol::PacketFlags::NONE);
        server->send_to_client(session_id, reinterpret_cast<const char*>(packet.data()), packet.size());
    });

    // Register game start callback with server (this will override the one in Multiplayer)
    // We need to call both start_game() and ensure players are spawned
    server->set_game_start_callback([this]() {
        this->start_game();
    });

    server_ecs_->init("lib/libECS.so");

    // Load logic systems (they won't run until game starts)
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libposition_system.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libcollision_system.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_Control.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_Shoot.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_GravitySys.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_EnemyCleanup.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_EnemyAI.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_Health.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_LifeTime.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_EnemySpawnSystem.so", DLLoader::LogicSystem);

    if (display_) {
        RenderManager::instance().init("R-Type Server", scale_, !windowed_);
        // Load render systems for display
        server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libanimation_system.so", DLLoader::RenderSystem);
        server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_Draw.so", DLLoader::RenderSystem);
        
        // Register and setup lobby state
        register_states();
        state_manager_.push_state("ServerLobby");
    }

    // Start network with worker threads
    if (!network_manager_->start(2)) {
        std::cerr << "[GameServer] Failed to start NetworkManager" << std::endl;
        return false;
    }

    running_ = true;
    std::cout << "[GameServer] Initialized successfully on port " << port_ << std::endl;
    return true;
}

void GameServer::run()
{
    std::cout << "[GameServer] Starting game loop..." << std::endl;
    run_tick_loop();
}

void GameServer::run_tick_loop()
{
    std::cout << "[GameServer] Entering authoritative tick loop (30Hz)" << std::endl;
    const std::chrono::milliseconds tick_duration(33); // ~30Hz

    while (running_) {
        auto tick_start = std::chrono::steady_clock::now();

        // Process incoming network packets and convert to ECS components
        server_ecs_->process_packets();

        // Only run game systems if game has started
        if (game_started_) {
            server_ecs_->tick(0.033f);
        }

        if (display_) {
            RenderManager::instance().begin_frame();
            
            // Update state manager (handles lobby UI)
            state_manager_.update(0.033f);
            
            // Render state manager (lobby or game UI)
            state_manager_.render();
            
            // If game started, also render game entities
            if (game_started_) {
                server_ecs_->GetDLLoader().update_all_systems(server_ecs_->GetRegistry(), 0.033f, DLLoader::RenderSystem);
            }
            
            RenderManager::instance().end_frame();
            if (WindowShouldClose()) {
                running_ = false;
            }
        }

        // Sleep until next tick
        auto elapsed = std::chrono::steady_clock::now() - tick_start;
        if (elapsed < tick_duration) {
            std::this_thread::sleep_for(tick_duration - elapsed);
        }

        // If this server is a machine-made instance, auto-shutdown when all clients
        // have disconnected and stayed disconnected for the grace period. This ensures
        // instances don't linger when nobody is connected (applies both in lobby and
        // during/after a running game).
        if (is_machine_made_ && running_) {
            auto server = network_manager_->get_server();
            if (server) {
                size_t client_count = server->get_client_count();
                if (client_count == 0) {
                    if (!zero_clients_timer_active_) {
                        zero_clients_timer_active_ = true;
                        zero_clients_since_ = std::chrono::steady_clock::now();
                        std::cout << Console::yellow("[GameServer] ") << "Instance on port " << port_ << " has 0 clients; starting shutdown grace timer (" << zero_clients_grace_.count() << "ms)" << std::endl;
                    } else {
                        auto since = std::chrono::steady_clock::now() - zero_clients_since_;
                        if (since >= zero_clients_grace_) {
                            std::cout << Console::yellow("[GameServer] ") << "Instance on port " << port_ << " no clients after grace period; shutting down instance" << std::endl;
                            running_ = false; // will break the run loop and cause instance to exit
                            // allow a final tick to flush any state
                        }
                    }
                } else {
                    // clients present again -> reset timer
                    if (zero_clients_timer_active_) {
                        zero_clients_timer_active_ = false;
                        std::cout << Console::cyan("[GameServer] ") << "Instance on port " << port_ << " clients reconnected; cancelling shutdown timer" << std::endl;
                    }
                }
            }
        }
    }

    std::cout << "[GameServer] Tick loop ended" << std::endl;
}

void GameServer::update(float delta)
{
    // Individual update call (if needed outside the main loop)
    (void)delta;
}

void GameServer::shutdown()
{
    std::cout << "[GameServer] Shutting down..." << std::endl;
    running_ = false;

    if (network_manager_) {
        network_manager_->stop();
    }

    server_ecs_.reset();
    message_queue_.reset();
    network_manager_.reset();
    io_context_.reset();

    if (display_) {
        RenderManager::instance().shutdown();
    }

    std::cout << "[GameServer] Shutdown complete" << std::endl;
}

void GameServer::register_states()
{
    std::cout << "[GameServer] Registering states..." << std::endl;
    
    // Create lobby state with UDP server reference
    auto server = network_manager_->get_server();
    state_manager_.register_state_with_factory("ServerLobby", [this, server]() -> std::shared_ptr<IGameState> {
        lobby_state_ = std::make_shared<ServerLobby>(server.get());
        return lobby_state_;
    });
}

void GameServer::start_game()
{
    std::cout << "[GameServer] Starting game! Transitioning from lobby..." << std::endl;

    game_started_ = true;
    
    // Generate and set random seed for deterministic gameplay
    std::random_device rd;
    unsigned int game_seed = rd();
    server_ecs_->GetRegistry().set_random_seed(game_seed);

    std::cout << "[GameServer] Generated game seed: " << game_seed << std::endl;

    // Broadcast the seed to all clients
    auto server = network_manager_->get_server();
    if (server) {
        RType::Protocol::GameSeed seed_msg;
        seed_msg.seed = game_seed;
        
        auto packet = RType::Protocol::create_packet(
            static_cast<uint8_t>(RType::Protocol::GameMessage::GAME_SEED),
            seed_msg
        );
        
        server->broadcast(reinterpret_cast<const char*>(packet.data()), packet.size());
        std::cout << "[GameServer] Game seed broadcasted to all clients" << std::endl;
    }
    
    // Spawn all players (was previously done in Multiplayer callback that got overwritten)
    if (server_ecs_->GetMultiplayer()) {
        server_ecs_->GetMultiplayer()->spawn_all_players();
    }
    
    // Mark lobby as game started (for clients to know)
    if (lobby_state_) {
        lobby_state_->set_game_started(true);
    }
    
    // Clear lobby UI if in display mode
    if (display_) {
        state_manager_.clear_states();
    }

    std::cout << "[GameServer] Game started! ECS systems now active." << std::endl;
}

void GameServer::handle_instance_request(const std::string &session_id) {
    std::lock_guard<std::mutex> lk(instances_mtx_);
    if (max_lobbies_ <= 0) {
        // Not supported
        send_instance_created(session_id, 0);
        return;
    }
    if (active_instances_ >= max_lobbies_) {
        std::cout << Console::yellow("[GameServer] ") << "Instance creation denied: max instances reached (" << active_instances_ << "/" << max_lobbies_ << ")" << std::endl;
        send_instance_created(session_id, 0);
        return;
    }

    // Find next available UDP port starting from base port + 1.
    // We'll probe ports until we find one that can be bound (and is not already in our instances_ list).
    const uint16_t start_port = static_cast<uint16_t>(port_ + 1);
    const uint16_t max_probe = 65535;
    uint16_t new_port = 0;
    for (uint32_t candidate = start_port; candidate <= max_probe; ++candidate) {
        uint16_t cand = static_cast<uint16_t>(candidate);
        // skip ports we already know about in instances_
        bool used = false;
        for (const auto &it : instances_) {
            if (it.port == cand) { used = true; break; }
        }
        if (used) continue;

        // Probe both UDP and TCP to avoid selecting a port already in use by either protocol.
        // We attempt to bind a UDP socket and a TCP acceptor on the candidate port. If both succeed,
        // consider the port free. Use error_code version to avoid exceptions and close handles on success.
        try {
            asio::io_context probe_ctx;
            asio::error_code ec_udp;
            asio::ip::udp::socket udp_sock(probe_ctx);
            udp_sock.open(asio::ip::udp::v4(), ec_udp);
            if (ec_udp) continue;
            asio::ip::udp::endpoint udp_ep(asio::ip::udp::v4(), cand);
            udp_sock.bind(udp_ep, ec_udp);
            if (ec_udp) {
                // UDP bind failed -> port in use for UDP
                udp_sock.close();
                continue;
            }

            asio::error_code ec_tcp;
            asio::ip::tcp::acceptor tcp_acc(probe_ctx);
            tcp_acc.open(asio::ip::tcp::v4(), ec_tcp);
            if (ec_tcp) {
                udp_sock.close();
                continue;
            }
            // Allow address reuse false to ensure exclusivity
            asio::ip::tcp::endpoint tcp_ep(asio::ip::tcp::v4(), cand);
            tcp_acc.bind(tcp_ep, ec_tcp);
            if (ec_tcp) {
                // TCP bind failed -> port in use for TCP
                tcp_acc.close();
                udp_sock.close();
                continue;
            }
            // If we reached here both binds succeeded: close and pick the port
            tcp_acc.close();
            udp_sock.close();
            new_port = cand;
            break;
        } catch (...) {
            // Any error -> skip candidate
            continue;
        }
    }

    if (new_port == 0) {
        std::cout << Console::yellow("[GameServer] ") << "Instance creation denied: no available ports found" << std::endl;
        send_instance_created(session_id, 0);
        return;
    }

    ++active_instances_;

    std::cout << Console::green("[GameServer] ") << "Spawning new instance on port " << new_port << std::endl;

    // Spawn instance in background thread
    std::thread t([this, new_port]() {
        try {
            // Create a headless GameServer instance (no display)
            auto srv = std::make_shared<GameServer>(false, false, 1.0f, 0, max_players_, true);
            srv->set_port(new_port);
            if (!srv->init()) {
                std::cout << Console::red("[GameServer] ") << "Failed to initialize instance on port " << new_port << std::endl;
                // Decrement active instances
                std::lock_guard<std::mutex> lk(instances_mtx_);
                --active_instances_;
                return;
            }

            // Run until the instance ends (init() -> run() blocks)
            srv->run();

            // When instance run returns, consider it terminated
            std::cout << Console::yellow("[GameServer] ") << "Instance on port " << new_port << " exited" << std::endl;
        } catch (...) {
            std::cout << Console::red("[GameServer] ") << "Exception while running instance on port " << new_port << std::endl;
        }

        std::lock_guard<std::mutex> lk(instances_mtx_);
        --active_instances_;
        // Remove instance info from list (if present)
        for (auto it = instances_.begin(); it != instances_.end(); ++it) {
            if (it->port == new_port) {
                instances_.erase(it);
                break;
            }
        }
        // Erase thread entry from map (we're running inside that thread)
        auto tit = instance_threads_.find(new_port);
        if (tit != instance_threads_.end()) {
            instance_threads_.erase(tit);
        }
        // Broadcast updated instance list to front clients
        try {
            broadcast_instance_list();
        } catch (...) {
            // best-effort
        }
    });

    // Store thread and detach (we only need active_instances_ count)
    instance_threads_.emplace(new_port, std::move(t));
    instance_threads_[new_port].detach();

    // Add to instances list and broadcast available instances to all connected clients
    RType::Protocol::InstanceInfo info{};
    info.port = new_port;
    info.status = 0; // waiting
    strncpy(info.name, "", sizeof(info.name)-1);
    instances_.push_back(info);
    broadcast_instance_list();

    // Reply to the requesting client with instance port
    send_instance_created(session_id, new_port);
    // Proactively disconnect the requesting session from the front server so it stops
    // receiving broadcasts from the front lobby. This avoids relying on the client's
    // unreliable UDP CLIENT_DISCONNECT reaching the front server in time.
    try {
        auto server_ptr = network_manager_->get_server();
        if (server_ptr) {
            auto sess = server_ptr->get_session(session_id);
            if (sess) {
                std::cout << Console::yellow("[GameServer] ") << "Disconnecting requesting session " << session_id << " from front server after instance creation" << std::endl;
                sess->disconnect();
            }
        }
    } catch (...) {
        // best-effort: don't crash the instance creation flow if this fails
    }
}

void GameServer::send_instance_created(const std::string &session_id, uint16_t port) {
    auto server = network_manager_->get_server();
    if (!server) return;

    RType::Protocol::InstanceCreated ic{};
    ic.port = port;
    auto packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::SystemMessage::INSTANCE_CREATED), ic, RType::Protocol::PacketFlags::RELIABLE);
    server->send_to_client(session_id, reinterpret_cast<const char*>(packet.data()), packet.size());
}

void GameServer::broadcast_instance_list() {
    auto server = network_manager_->get_server();
    if (!server) return;
    RType::Protocol::InstanceList il{};
    il.instance_count = static_cast<uint8_t>(std::min<size_t>(instances_.size(), 8));
    for (size_t i = 0; i < il.instance_count; ++i) {
        il.instances[i] = instances_[i];
    }
    auto packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::SystemMessage::INSTANCE_LIST), il, RType::Protocol::PacketFlags::NONE);
    server->broadcast(reinterpret_cast<const char*>(packet.data()), packet.size());
}

