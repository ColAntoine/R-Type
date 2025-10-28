/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PUpAnimationSys system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include "ECS/Components/IComponent.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Renderer/RenderManager.hpp"

#include "ECS/Components/Sprite.hpp"
#include "ECS/Components/Position.hpp"

#include "ECS/UI/UIBuilder.hpp"
#include "ECS/UI/Components/Text.hpp"
#include "ECS/Components/UIComponent.hpp"

#include "Entity/Components/PUpAnimation/PUpAnimation.hpp"

#include "Constants.hpp"

#include <queue>
#include <vector>

struct animationArrow : IComponent {};
struct animationText : IComponent {};

class PUpAnimationSys : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "PUpAnimationSys"; }
private:
    void getAnimations(registry &r);
    void updateAnimation(registry &r, float dt);
    bool pendingAnimation(registry &r);

    std::queue<PUpAnimation> _animationQueue;
    PUpAnimation _current;
    bool _isDone{true};
    entity _arrowEnt{0};
    float _arrowBaseY{0.0f};
    entity _textEnt{0};
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
