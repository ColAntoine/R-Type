/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** FollowingSys system implementation
*/

#include <memory>
#include <cmath>

#include "FollowingSys.hpp"

void FollowingSys::update(registry& r, float dt)
{
    updatePlayerPos(r);
    updateProjTrajectories(r);
}

void FollowingSys::updatePlayerPos(registry &r)
{
    auto posArr = r.get_if<position>();
    auto playerArr = r.get_if<Player>();
    auto remoteArr = r.get_if<remote_player>();

    if (!posArr) return;

    // First, try to find a local Player position
    if (playerArr) {
        for (auto [pos, player, ent]: zipper(*posArr, *playerArr)) {
            _playerPos = pos;
            return;
        }
    }

    // If no local Player found, try to find a remote_player position
    if (remoteArr) {
        for (auto [pos, remote, ent]: zipper(*posArr, *remoteArr)) {
            _playerPos = pos;
            return;
        }
    }
}

void FollowingSys::updateProjTrajectories(registry &r)
{
    auto posArr = r.get_if<position>();
    auto velArr = r.get_if<velocity>();
    auto followArr = r.get_if<Following>();

    if (!posArr || !velArr || !followArr) return;

    for (auto [pos, vel, follow, ent] : zipper(*posArr, *velArr, *followArr)) {
        float dx = _playerPos.x - pos.x;
        float dy = _playerPos.y - pos.y;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist > 0) {
            vel.vx = (dx / dist) * follow._followForce;
            vel.vy = (dy / dist) * follow._followForce;
        }
    }
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new FollowingSys();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}
