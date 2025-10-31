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
#include "ECS/Messaging/MessagingManager.hpp"

#include "Constants.hpp"

#include <queue>
#include <vector>

struct animationArrow : IComponent {};
struct animationText : IComponent {};

class PUpAnimationSys : public ISystem {
public:
    PUpAnimationSys();
    ~PUpAnimationSys() override;

    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "PUpAnimationSys"; }
private:
    void getAnimations(registry &r);
    void updateAnimation(registry &r, float dt);
    bool pendingAnimation(registry &r);

    void createArrow(registry &r);
    void createText(registry &r);

    std::queue<PUpAnimation> _animationQueue;
    PUpAnimation _current;
    bool _isDone{true};
    entity _arrowEnt{0};
    float _arrowBaseY{0.0f};
    entity _textEnt{0};

    EventBus::CallbackId _themeChangedCallbackId{0};
};


#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);
