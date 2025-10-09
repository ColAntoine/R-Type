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
};