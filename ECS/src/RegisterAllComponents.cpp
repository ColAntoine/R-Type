#include "ECS/RegisterAllComponents.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Components/Sprite.hpp"
#include "ECS/Components/Animation.hpp"
#include "ECS/Components/Collider.hpp"
#include "ECS/Components/NetInput.hpp"

void RegisterAllComponents(registry &r) {
    r.register_component<position>();
    r.register_component<velocity>();
    r.register_component<sprite>();
    r.register_component<animation>();
    r.register_component<collider>();
    r.register_component<NetInput>();
}
