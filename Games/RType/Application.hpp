#pragma once

#include "Core/EventManager.hpp"
#include "Core/Services/ServiceManager.hpp"
#include "Core/Events.hpp"
#include "Core/Services/Input/Input.hpp"
#include "Core/Services/Network/Network.hpp"
#include "Core/Services/Render/Render.hpp"
#include "Core/Systems/Input/Input.hpp"
#include "Core/Systems/Network/Network.hpp"
#include "Core/States/GameStateManager.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Entity.hpp"
#include "ECS/ComponentFactory.hpp"
#include <memory>
#include <sstream>
#include <iostream>
#include "ECS/ILoader.hpp"
#include "ECS/LinuxLoader.hpp"
#include "ECS/WindowsLoader.hpp"
#include "ECS/MacOs.hpp"

#define PLAYER_WIDTH 60.0f
#define PLAYER_HEIGHT 40.0f

class Application {
    private:
        // Core systems
        EventManager event_manager_;
        ServiceManager service_manager_;

        // ECS
        registry ecs_registry_;
        entity local_player_entity_;
        std::unique_ptr<ILoader> system_loader_;
        IComponentFactory* component_factory_;

        // ECS Systems
        std::unique_ptr<InputSystem> input_system_;
        std::unique_ptr<NetworkSystem> network_system_;

        // Game State Management
        GameStateManager state_manager_;

        // Game state
        bool running_ = false;
        int local_player_id_ = 0;
        std::string player_name_ = "Player";

    public:
        Application() : component_factory_(nullptr) {}
        ~Application() = default;

        bool initialize();
        void run();
        void shutdown();

        // Connection management (moved from initialize)
        bool connect_to_server(const std::string& server_ip, int server_port);
        void send_ready_signal(bool ready);

    private:
        void setup_ecs();
        void load_systems();
        void service_setup();
        void setup_event_handlers();
        void setup_game_states();

    public:
        // Getters for states to access application systems
        EventManager& get_event_manager() { return event_manager_; }
        ServiceManager& get_service_manager() { return service_manager_; }
        registry& get_ecs_registry() { return ecs_registry_; }
        IComponentFactory* get_component_factory() { return component_factory_; }
        entity get_local_player_entity() const { return local_player_entity_; }
        int get_local_player_id() const { return local_player_id_; }
        const std::string& get_player_name() const { return player_name_; }
        void set_player_name(const std::string& name) {
            // Limit player name to 31 characters (32-1 for null terminator)
            if (name.length() > 31) {
                player_name_ = name.substr(0, 31);
            } else {
                player_name_ = name.empty() ? "Player" : name;
            }
        }
        ILoader& get_system_loader() { return *system_loader_; }

        // ECS update methods for states to use
        void update_ecs_systems(float delta_time);
        void update_traditional_ecs_systems(float delta_time);
        std::string getExtention();
};