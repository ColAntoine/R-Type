/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Application (Client)
*/

#pragma once

#include "Core/GameCore.hpp"
#include "Core/Client/EventManager.hpp"
#include "Core/Services/ServiceManager.hpp"
#include "Core/Services/Input/Input.hpp"
#include "Core/Services/Network/Network.hpp"
#include "Core/Services/Render/Render.hpp"
#include "Core/States/GameStateManager.hpp"
#include "Core/Systems/Input/Input.hpp"
#include "Core/Systems/Network/Network.hpp"
#include <memory>
#include <string>

/**
 * @brief Client application (with UI, rendering, input)
 */
class Application : public GameCore {
private:
    // Client-specific
    EventManager event_manager_;
    ServiceManager service_manager_;
    GameStateManager state_manager_;

    std::unique_ptr<InputSystem> input_system_;
    std::unique_ptr<NetworkSystem> network_system_;

    entity local_player_entity_;
    int local_player_id_{0};
    std::string player_name_{"Player"};
    bool running_{false};

public:
    Application() = default;
    ~Application() override = default;

    bool initialize();
    void run();
    void shutdown();

    // Network
    bool connect_to_server(const std::string& server_ip, int server_port);
    void send_ready_signal(bool ready);

    // Getters
    EventManager& get_event_manager() { return event_manager_; }
    ServiceManager& get_service_manager() { return service_manager_; }
    GameStateManager& get_state_manager() { return state_manager_; }

    entity get_local_player() const { return local_player_entity_; }
    entity get_local_player_entity() const { return local_player_entity_; }  // ✅ Alias
    int get_local_player_id() const { return local_player_id_; }

    registry& get_ecs_registry() { return ecs_registry_; }
    const registry& get_ecs_registry() const { return ecs_registry_; }

    const std::string& get_player_name() const { return player_name_; }
    void set_player_name(const std::string& name) { player_name_ = name; }

    // ECS update methods
    void update_ecs_systems(float delta_time);
    void update_traditional_ecs_systems(float delta_time);

protected:
    /**
     * @brief Load client-specific systems (override from GameCore)
     */
    void load_specific_systems() override;

private:
    void service_setup();
    void setup_event_handlers();
    void setup_game_states();
    void setup_player_entity();
};