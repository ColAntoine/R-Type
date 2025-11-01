/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WaveSys system implementation
*/

#include <memory>

#include "WaveSys.hpp"

void WaveSys::update(registry &r, float dt)
{
    updateTimer(r, dt);
    handleStop(r);
    handleExplosion(r);
}

void WaveSys::updateTimer(registry &r, float dt)
{
    auto waveArr = r.get_if<WaveShoot>();

    if (!waveArr) return;

    for (auto [wave, ent]: zipper(*waveArr)) {
        wave._lifeTime += dt;
    }
}

void WaveSys::handleStop(registry &r)
{
    auto waveArr = r.get_if<WaveShoot>();
    auto velArr = r.get_if<velocity>();
    auto posArr = r.get_if<position>();

    if (!waveArr || !velArr || !posArr) return;

    for (auto [wave, vel, pos, ent]: zipper(*waveArr, *velArr, *posArr)) {
        if (wave._isStoped) {
            pos = wave._stopedPos;
            continue;
        }
        if (wave._lifeTime >= wave._expTimeout - 1.f) {
            vel.vx = 0.f;
            vel.vy = 0.f;
            wave._isStoped = true;
            wave._stopedPos = pos;
        }
    }
}

void WaveSys::handleExplosion(registry &r)
{
    auto waveArr = r.get_if<WaveShoot>();
    auto weapArr = r.get_if<Weapon>();
    std::vector<entity> entToKill;

    if (!waveArr) return;

    for (auto [wave, weap, ent]: zipper(*waveArr, *weapArr)) {
        if (wave._lifeTime >= wave._expTimeout) {
            if (wave._didExplode)
                entToKill.push_back(entity(ent));
            weap._wantsToFire = true;
            wave._didExplode = true;
        }
    }
    for (auto &ent: entToKill) {
        r.kill_entity(ent);
    }
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new WaveSys();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}
