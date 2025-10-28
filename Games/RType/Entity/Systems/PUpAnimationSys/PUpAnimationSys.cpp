/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PUpAnimationSys system implementation
*/

#include <memory>

#include "PUpAnimationSys.hpp"

void PUpAnimationSys::update(registry& r, float dt)
{
    getAnimations(r);
    updateAnimation(r);
}

void PUpAnimationSys::getAnimations(registry &r)
{
    auto pUpArr = r.get_if<PUpAnimation>();
    std::vector<entity> entToKill;

    if (!pUpArr) return;

    for (auto [pUp, ent] : zipper(*pUpArr)) {
        _animationQueue.push(pUp);
        entToKill.push_back(entity(ent));
    }

    for (auto ent : entToKill) {
        r.kill_entity(ent);
    }
}

void PUpAnimationSys::updateAnimation(registry &r)
{
    RenderManager &renderManager = RenderManager::instance();

    if (!_isDone) return;
    if (_isDone && !_animationQueue.empty()) {
        _current = _animationQueue.front();
        _animationQueue.pop();

        std::cout << "SPRITE ADDED" << std::endl;
        auto sEnt = r.spawn_entity();
        r.emplace_component<sprite>(sEnt, std::string(RTYPE_PATH_ASSETS) + "pUpArrow.gif", 500.f, 500.f);
        r.emplace_component<animationArrow>(sEnt);
        r.emplace_component<position>(sEnt, renderManager.scalePosX(40), renderManager.scalePosY(75));
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<PUpAnimationSys>();
    }
}
