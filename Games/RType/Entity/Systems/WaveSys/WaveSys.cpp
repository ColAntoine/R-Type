/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WaveSys system implementation
*/

#include <memory>

#include "WaveSys.hpp"

void WaveSys::update(registry& r, float dt)
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
    std::vector<entity> entToKill;

    if (!waveArr) return;

    for (auto [wave, ent]: zipper(*waveArr)) {
        if (wave._lifeTime >= wave._expTimeout) {
            auto wEnt = r.spawn_entity();
            Weapon w(
                wEnt,
                {"explode"},
                1.f,
                10.f,
                600.f,
                -1,
                true
            );
            w._wantsToFire = true;
            w._automatic = true;
            r.emplace_component<Weapon>(wEnt, w);
            r.emplace_component<position>(wEnt, wave._stopedPos);
            entToKill.push_back(entity(ent));
        }
    }
    for (auto &ent: entToKill) {
        r.kill_entity(ent);
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<WaveSys>();
    }
}
