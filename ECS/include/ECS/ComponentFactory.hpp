#pragma once

#include "ECS/Components/IComponent.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Registry.hpp"
#include <memory>

class IComponentFactory {
public:
    virtual ~IComponentFactory() = default;

    // Allow to add more components generically from external code
    template<typename ComponentType, typename... Args>
    void create_component(registry& reg, const entity& e, Args&&... args) {
        reg.emplace_component<ComponentType>(e, std::forward<Args>(args)...);
    }

    // Factory methods for creating components
    virtual void create_position(registry& reg, const entity& e, float x, float y) = 0;
    virtual void create_velocity(registry& reg, const entity& e, float vx, float vy) = 0;
    virtual void create_collider(registry& reg, const entity& e, float w, float h,
        float ox = 0.f, float oy = 0.f, bool trigger = false) = 0;
    virtual void create_sprite(registry& reg, const entity& e, const std::string& texture_path,
        float w, float h, float scale_x = 1.0f, float scale_y = 1.0f) = 0;
        // virtual void create_drawable(registry& reg, const entity& e, float w, float h,
    //                             unsigned char r = 255, unsigned char g = 255,
    //                             unsigned char b = 255, unsigned char a = 255) = 0;
    // virtual void create_controllable(registry& reg, const entity& e, float speed) = 0;
    // virtual void create_enemy(registry& reg, const entity& e, int enemy_type, float health = 1.0f) = 0;
    // virtual void create_remote_player(registry& reg, const entity& e, const std::string& client_id) = 0;
};