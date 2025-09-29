/*
** EPITECH PROJECT, 2025
** asd
** File description:
** da
*/

#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "ecs/component_factory.hpp"

position::position() : x(0), y(0) {}
position::position(float nx, float ny) : x(nx), y(ny) {}

velocity::velocity() : vx(0), vy(0) {}
velocity::velocity(float nvx, float nvy) : vx(nvx), vy(nvy) {}

drawable::drawable() {}
drawable::drawable(float nw, float nh, unsigned char nr, unsigned char ng, unsigned char nb, unsigned char na) : r(nr), g(ng), b(nb), a(na), w(nw), h(nh) {}

controllable::controllable() {}
controllable::controllable(float s) : speed(s) {}

collider::collider() {}
collider::collider(float nw, float nh, float ox, float oy, bool trig) : w(nw), h(nh), offset_x(ox), offset_y(oy), is_trigger(trig) {}

remote_player::remote_player() {}
remote_player::remote_player(const std::string& id) : client_id(id) {}

enemy::enemy() {}
enemy::enemy(int type, float hp) : enemy_type(type), health(hp) {}

lifetime::lifetime() {}
lifetime::lifetime(float max_time, bool collision_destroy, bool offscreen_destroy) 
    : max_lifetime(max_time), destroy_on_collision(collision_destroy), destroy_offscreen(offscreen_destroy) {}

spawner::spawner() {}
spawner::spawner(float interval, int max_ents) : spawn_interval(interval), max_entities(max_ents) {}

class ComponentFactory : public IComponentFactory {
    public:
        void create_position(registry& reg, const entity& e, float x, float y) override
        {
            reg.emplace_component<position>(e, x, y);
        }
        void create_velocity(registry& reg, const entity& e, float vx, float vy) override
        {
            reg.emplace_component<velocity>(e, vx, vy);
        }
        void create_drawable(registry& reg, const entity& e, float w, float h, unsigned char r, unsigned char g, unsigned char b, unsigned char a) override
        {
            reg.emplace_component<drawable>(e, w, h, r, g, b, a);
        }
        void create_controllable(registry& reg, const entity& e, float speed) override
        {
            reg.emplace_component<controllable>(e, speed);
        }
        void create_collider(registry& reg, const entity& e, float w, float h, float ox, float oy, bool trigger) override
        {
            reg.emplace_component<collider>(e, w, h, ox, oy, trigger);
        }
};

static ComponentFactory factory_instance;

extern "C" void register_components(registry &r) {
    r.register_component<position>();
    r.register_component<velocity>();
    r.register_component<drawable>();
    r.register_component<controllable>();
    r.register_component<collider>();
    r.register_component<remote_player>();
    r.register_component<enemy>();
    r.register_component<lifetime>();
    r.register_component<spawner>();
}

extern "C" IComponentFactory* get_component_factory() {
    return &factory_instance;
}