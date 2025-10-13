/*
** EPITECH PROJECT, 2025
** asd
** File description:
** da
*/

#include "ECS/Components.hpp"
#include "ECS/Registry.hpp"
#include "ECS/ComponentFactory.hpp"

class ComponentFactory : public IComponentFactory {
    public:
        static ComponentFactory &getInstance() {
            static ComponentFactory instance;

            return instance;
        }

    private:
        ComponentFactory() = default;
        ~ComponentFactory() = default;
};

extern "C" void register_components(registry &r) {
    r.register_component<position>();
    r.register_component<velocity>();
    // r.register_component<drawable>();
    // r.register_component<controllable>();
    r.register_component<collider>();
    // r.register_component<remote_player>();
    // r.register_component<Enemy>();
    // r.register_component<lifetime>();
    // r.register_component<spawner>();
    r.register_component<sprite>();
    r.register_component<animation>();
}

extern "C"
IComponentFactory* get_component_factory() {
    return &ComponentFactory::getInstance();
}