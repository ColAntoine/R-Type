#pragma once

#include "core/event_manager.hpp"
#include "core/service_manager.hpp"
#include "core/events.hpp"
#include "services/input_service.hpp"
#include "services/network_service.hpp"
#include "services/render_service.hpp"
#include "systems/input_system.hpp"
#include "systems/network_systems.hpp"
#include "ecs/registry.hpp"
#include "ecs/entity.hpp"
#include "ecs/dlloader.hpp"
#include "ecs/component_factory.hpp"
#include <memory>
#include <sstream>
#include <iostream>

#define PLAYER_WIDTH 40.0f
#define PLAYER_HEIGHT 40.0f

class Application {
    private:
        // Core systems
        EventManager event_manager_;
        ServiceManager service_manager_;

        // ECS
        registry ecs_registry_;
        entity local_player_entity_;
        DLLoader system_loader_;
        IComponentFactory* component_factory_;

        // ECS Systems
        std::unique_ptr<InputSystem> input_system_;
        std::unique_ptr<NetworkSystem> network_system_;

        // Game state
        bool running_ = false;
        int local_player_id_ = 0;

    public:
        Application() : component_factory_(nullptr) {}
        ~Application() = default;

        bool initialize(const std::string& server_ip = "127.0.0.1", int server_port = 8080);
        void run();
        void shutdown();

    private:
        void setup_ecs();
        void load_systems();
        void setup_event_handlers();
        void update_ecs_systems(float delta_time);
        void update_traditional_ecs_systems(float delta_time);
        void render_frame();
};