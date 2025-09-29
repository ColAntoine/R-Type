#include "server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cerrno>
#include <sys/time.h>
#include <chrono>

// Initialize static counters
int UDPServer::next_player_id = 1;
uint32_t UDPServer::next_entity_id = 1000; // Start enemies at 1000 to avoid conflict with player IDs

UDPServer::UDPServer() : socket_fd(-1), port(0), running(false) {
    memset(&server_address, 0, sizeof(server_address));
    last_spawn_time = std::chrono::steady_clock::now();
    last_network_update = std::chrono::steady_clock::now();
}

UDPServer::~UDPServer() {
    stop();
}

bool UDPServer::initialize(int server_port) {
    int opt = 1;
    port = server_port;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return false;
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Error setting socket options" << std::endl;
        close(socket_fd);
        socket_fd = -1;
        return false;
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);
    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Error binding socket to port " << port << std::endl;
        close(socket_fd);
        socket_fd = -1;
        return false;
    }
    // Set receive timeout so recvfrom doesn't block indefinitely and allows clean shutdown
    struct timeval tv;
    tv.tv_sec = 1; // 1 second timeout
    tv.tv_usec = 0;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
        std::cerr << "Warning: could not set socket recv timeout" << std::endl;
        // Not fatal
    }
    std::cout << "UDP Server initialized on port " << port << std::endl;
    return true;
}

bool UDPServer::start() {
    if (socket_fd < 0) {
        std::cerr << "Server not initialized. Call initialize() first." << std::endl;
        return false;
    }

    if (running.load()) {
        std::cout << "Server is already running" << std::endl;
        return true;
    }

    running.store(true);
    server_thread = std::thread(&UDPServer::server_loop, this);

    std::cout << "UDP Server started and listening on port " << port << std::endl;
    return true;
}

void UDPServer::stop() {
    if (running.load()) {
        running.store(false);
        if (server_thread.joinable())
            server_thread.join();
        std::cout << "Server stopped" << std::endl;
    }
    if (socket_fd >= 0) {
        close(socket_fd);
        socket_fd = -1;
    }
    clients.clear();
}

bool UDPServer::is_running() const {
    return running.load();
}

void UDPServer::server_loop() {
    char buffer[1024];
    sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    time_t last_cleanup = time(nullptr);
    auto last_game_update = std::chrono::steady_clock::now();

    std::cout << "Server loop started" << std::endl;

    while (running.load()) {
        auto current_time_chrono = std::chrono::steady_clock::now();
        auto dt = std::chrono::duration<float>(current_time_chrono - last_game_update).count();
        last_game_update = current_time_chrono;

        // Update game logic
        update_enemies(dt);
        check_collisions();

        time_t current_time = time(nullptr);
        if (current_time - last_cleanup > 5) {
            cleanup_disconnected_clients();
            last_cleanup = current_time;
        }
        memset(buffer, 0, sizeof(buffer));
        memset(&client_address, 0, sizeof(client_address));

        ssize_t bytes_received = recvfrom(socket_fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_address, &client_len);
        if (bytes_received < 0) {
            // timeout or interrupted syscall - check running flag and continue
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                continue;
            }
            std::cerr << "recvfrom error: " << strerror(errno) << std::endl;
            break;
        }

        if (bytes_received == 0) {
            continue;
        }
        buffer[bytes_received] = '\0';
        std::string client_key = get_client_key(client_address);
        if (clients.find(client_key) == clients.end()) {
            add_client(client_address);
        }
        clients[client_key].last_ping = time(nullptr);
        handle_client_message(buffer, bytes_received, client_address);
    }

    std::cout << "Server loop ended" << std::endl;
}

std::string UDPServer::get_client_key(const sockaddr_in& client_addr) {
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
    return std::string(ip_str) + ":" + std::to_string(ntohs(client_addr.sin_port));
}

void UDPServer::handle_client_message(const char* buffer, int length, const sockaddr_in& client_addr) {
    std::string client_key = get_client_key(client_addr);

    // Try to parse as binary packet first
    Protocol::PacketHeader header;
    const uint8_t* payload;

    if (Protocol::parse_packet(reinterpret_cast<const uint8_t*>(buffer), length, header, payload)) {
        if (header.message_type == static_cast<uint8_t>(Protocol::RTypeMessage::POSITION_UPDATE)) {
            if (header.payload_size >= sizeof(Protocol::PositionUpdate)) {
                const Protocol::PositionUpdate* pos_update = reinterpret_cast<const Protocol::PositionUpdate*>(payload);
                handle_position_update(client_key, *pos_update);
            }
        }
    }
}

void UDPServer::add_client(const sockaddr_in& client_addr) {
    std::string client_key = get_client_key(client_addr);

    ClientInfo client_info;
    client_info.address = client_addr;

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
    client_info.ip = std::string(ip_str);
    client_info.port = ntohs(client_addr.sin_port);
    client_info.connected = true;
    client_info.last_ping = time(nullptr);

    client_info.x = 100.0f;
    client_info.y = 100.0f;
    client_info.player_id = next_player_id++;
    clients[client_key] = client_info;

    std::cout << "New client connected: " << client_key << " (Player ID: " << client_info.player_id << ")" << std::endl;
    std::cout << "Total clients: " << clients.size() << std::endl;

    Protocol::ServerAccept accept_msg;
    accept_msg.player_id = client_info.player_id;
    accept_msg.server_version = 1;
    accept_msg.server_capabilities = 0;

    uint8_t buffer[sizeof(Protocol::PacketHeader) + sizeof(Protocol::ServerAccept)];
    size_t packet_size = Protocol::create_packet(buffer,
                                                static_cast<uint8_t>(Protocol::SystemMessage::SERVER_ACCEPT),
                                                &accept_msg, sizeof(accept_msg));
    send_to_client(client_key, reinterpret_cast<const char*>(buffer), packet_size);
    send_existing_players_to_client(client_key);
}

void UDPServer::handle_position_update(const std::string& client_key, const Protocol::PositionUpdate& pos_update) {
    auto client_it = clients.find(client_key);
    if (client_it == clients.end()) return;

    clients[client_key].x = pos_update.position_x;
    clients[client_key].y = pos_update.position_y;

    Protocol::EntityUpdate update;
    update.entity_id = clients[client_key].player_id;
    update.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    update.position_x = pos_update.position_x;
    update.position_y = pos_update.position_y;
    update.velocity_x = 0.0f;
    update.velocity_y = 0.0f;
    update.state_flags = 0;

    uint8_t buffer[sizeof(Protocol::PacketHeader) + sizeof(Protocol::EntityUpdate)];
    size_t packet_size = Protocol::create_packet(buffer,
                                                static_cast<uint8_t>(Protocol::RTypeMessage::ENTITY_UPDATE),
                                                &update, sizeof(update));

    for (const auto& client_pair : clients) {
        if (client_pair.first != client_key) {
            send_to_client(client_pair.first, reinterpret_cast<const char*>(buffer), packet_size);
        }
    }
}

void UDPServer::remove_client(const std::string& client_key) {
    auto it = clients.find(client_key);
    if (it != clients.end()) {
        int disconnected_player_id = it->second.player_id;

        Protocol::PlayerDisconnect disconnect_msg;
        disconnect_msg.player_id = disconnected_player_id;

        uint8_t buffer[sizeof(Protocol::PacketHeader) + sizeof(Protocol::PlayerDisconnect)];
        size_t packet_size = Protocol::create_packet(buffer,
                                                    static_cast<uint8_t>(Protocol::SystemMessage::PLAYER_DISCONNECT),
                                                    &disconnect_msg, sizeof(disconnect_msg));

        for (const auto& client_pair : clients) {
            if (client_pair.first != client_key) {
                send_to_client(client_pair.first, reinterpret_cast<const char*>(buffer), packet_size);
            }
        }

        clients.erase(it);
        std::cout << "Client disconnected: " << client_key << " (Player ID: " << disconnected_player_id << ")" << std::endl;
        std::cout << "Total clients: " << clients.size() << std::endl;
    }
}

void UDPServer::cleanup_disconnected_clients() {
    time_t current_time = time(nullptr);
    const int TIMEOUT_SECONDS = 10;
    std::vector<std::string> clients_to_remove;
    for (const auto& client_pair : clients) {
        if (current_time - client_pair.second.last_ping > TIMEOUT_SECONDS) {
            clients_to_remove.push_back(client_pair.first);
        }
    }
    for (const std::string& client_key : clients_to_remove) {
        std::cout << "Removing inactive client: " << client_key << std::endl;
        remove_client(client_key);
    }
}

void UDPServer::send_to_client(const std::string& client_key, const char* data, int length) {
    auto it = clients.find(client_key);
    if (it != clients.end()) {
        ssize_t bytes_sent = sendto(socket_fd, data, length, 0,
                                  (struct sockaddr*)&(it->second.address),
                                  sizeof(it->second.address));

        if (bytes_sent < 0) {
            std::cerr << "Error sending data to client " << client_key << std::endl;
        }
    }
}

void UDPServer::send_to_all_clients(const char* data, int length) {
    for (const auto& client_pair : clients) {
        send_to_client(client_pair.first, data, length);
    }
}

void UDPServer::broadcast_message(const std::string& message) {
    std::string broadcast_msg = "Broadcast: " + message;
    send_to_all_clients(broadcast_msg.c_str(), broadcast_msg.length());
    std::cout << "Broadcasted message to " << clients.size() << " clients" << std::endl;
}

int UDPServer::get_client_count() const {
    return clients.size();
}

std::vector<std::string> UDPServer::get_connected_clients() const {
    std::vector<std::string> client_list;
    for (const auto& client_pair : clients) {
        client_list.push_back(client_pair.first);
    }
    return client_list;
}

void UDPServer::print_server_info() const {
    std::cout << "=== UDP Server Info ===" << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "Running: " << (running.load() ? "Yes" : "No") << std::endl;
    std::cout << "Connected clients: " << clients.size() << std::endl;

    if (!clients.empty()) {
        std::cout << "Client list:" << std::endl;
        for (const auto& client_pair : clients) {
            const ClientInfo& info = client_pair.second;
            std::cout << "  - " << client_pair.first
                      << " (last ping: " << (time(nullptr) - info.last_ping) << "s ago)" << std::endl;
        }
    }
    std::cout << "======================" << std::endl;
}

void UDPServer::send_existing_players_to_client(const std::string& client_key) {
    for (const auto& client_pair : clients) {
        if (client_pair.first != client_key) {
            Protocol::EntityUpdate update;
            update.entity_id = client_pair.second.player_id;
            update.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
            update.position_x = client_pair.second.x;
            update.position_y = client_pair.second.y;
            update.velocity_x = 0.0f;
            update.velocity_y = 0.0f;
            update.state_flags = 0;

            uint8_t buffer[sizeof(Protocol::PacketHeader) + sizeof(Protocol::EntityUpdate)];
            size_t packet_size = Protocol::create_packet(buffer,
                                                        static_cast<uint8_t>(Protocol::RTypeMessage::ENTITY_UPDATE),
                                                        &update, sizeof(update));
            send_to_client(client_key, reinterpret_cast<const char*>(buffer), packet_size);
        }
    }
}

void UDPServer::update_enemies(float dt) {
    auto current_time = std::chrono::steady_clock::now();
    
    // Spawn enemies every 3 seconds
    if (std::chrono::duration<float>(current_time - last_spawn_time).count() >= 3.0f && enemies.size() < 5) {
        spawn_enemy();
        last_spawn_time = current_time;
    }
    
    // Update enemy positions and remove expired ones
    std::vector<uint32_t> enemies_to_remove;
    
    for (auto& [enemy_id, enemy] : enemies) {
        // Update position
        enemy.x += enemy.vx * dt;
        enemy.y += enemy.vy * dt;
        
        // Check if enemy should be removed (offscreen or expired)
        float lifetime = std::chrono::duration<float>(current_time - enemy.spawn_time).count();
        if (enemy.x < -50 || lifetime > 15.0f) {
            enemies_to_remove.push_back(enemy_id);
        }
    }
    
    // Only broadcast enemy positions at 20 FPS to reduce network traffic
    float network_dt = std::chrono::duration<float>(current_time - last_network_update).count();
    if (network_dt >= 0.05f) { // 20 FPS (1/20 = 0.05 seconds)
        for (auto& [enemy_id, enemy] : enemies) {
            if (std::find(enemies_to_remove.begin(), enemies_to_remove.end(), enemy_id) == enemies_to_remove.end()) {
                // Broadcast enemy position to all clients
                Protocol::EntityUpdate update;
                update.entity_id = enemy_id;
                update.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                    current_time.time_since_epoch()).count();
                update.position_x = enemy.x;
                update.position_y = enemy.y;
                update.velocity_x = enemy.vx;
                update.velocity_y = enemy.vy;
                update.state_flags = 1; // Flag to indicate this is an enemy
                
                uint8_t buffer[sizeof(Protocol::PacketHeader) + sizeof(Protocol::EntityUpdate)];
                size_t packet_size = Protocol::create_packet(buffer, 
                                                            static_cast<uint8_t>(Protocol::RTypeMessage::ENTITY_UPDATE),
                                                            &update, sizeof(update));
                
                for (const auto& client_pair : clients) {
                    send_to_client(client_pair.first, reinterpret_cast<const char*>(buffer), packet_size);
                }
            }
        }
        last_network_update = current_time;
    }
    
    // Remove expired enemies
    for (uint32_t enemy_id : enemies_to_remove) {
        // Send destruction notification to all clients
        Protocol::EntityDestroy destroy;
        destroy.entity_id = enemy_id;
        destroy.entity_type = 1; // Enemy
        
        uint8_t buffer[sizeof(Protocol::PacketHeader) + sizeof(Protocol::EntityDestroy)];
        size_t packet_size = Protocol::create_packet(buffer, 
                                                    static_cast<uint8_t>(Protocol::RTypeMessage::ENTITY_DESTROY),
                                                    &destroy, sizeof(destroy));
        
        for (const auto& client_pair : clients) {
            send_to_client(client_pair.first, reinterpret_cast<const char*>(buffer), packet_size);
        }
        
        enemies.erase(enemy_id);
        std::cout << "Destroyed enemy " << enemy_id << std::endl;
    }
}

void UDPServer::spawn_enemy() {
    Enemy enemy;
    enemy.entity_id = next_entity_id++;
    enemy.x = 850; // Spawn off-screen right
    enemy.y = 50 + (rand() % 500); // Random Y position
    enemy.vx = -100.0f; // Move left
    enemy.vy = 0.0f;
    enemy.health = 1.0f;
    enemy.enemy_type = 0;
    enemy.spawn_time = std::chrono::steady_clock::now();
    
    enemies[enemy.entity_id] = enemy;
    
    std::cout << "Spawned enemy " << enemy.entity_id << " at (" << enemy.x << ", " << enemy.y << ")" << std::endl;
}

void UDPServer::check_collisions() {
    std::vector<uint32_t> enemies_to_remove;
    
    // Check player-enemy collisions
    for (const auto& [client_key, client_info] : clients) {
        for (const auto& [enemy_id, enemy] : enemies) {
            float dx = client_info.x - enemy.x;
            float dy = client_info.y - enemy.y;
            float distance_sq = dx * dx + dy * dy;
            
            // Simple collision detection (assuming 40x40 player and 30x30 enemy)
            if (distance_sq < (35 * 35)) { // Collision threshold
                enemies_to_remove.push_back(enemy_id);
                std::cout << "Collision: Player " << client_info.player_id << " hit enemy " << enemy_id << std::endl;
            }
        }
    }
    
    // Remove collided enemies
    for (uint32_t enemy_id : enemies_to_remove) {
        // Send destruction notification to all clients
        Protocol::EntityDestroy destroy;
        destroy.entity_id = enemy_id;
        destroy.entity_type = 1; // Enemy
        
        uint8_t buffer[sizeof(Protocol::PacketHeader) + sizeof(Protocol::EntityDestroy)];
        size_t packet_size = Protocol::create_packet(buffer, 
                                                    static_cast<uint8_t>(Protocol::RTypeMessage::ENTITY_DESTROY),
                                                    &destroy, sizeof(destroy));
        
        for (const auto& client_pair : clients) {
            send_to_client(client_pair.first, reinterpret_cast<const char*>(buffer), packet_size);
        }
        
        enemies.erase(enemy_id);
        std::cout << "Enemy " << enemy_id << " destroyed by collision" << std::endl;
    }
}