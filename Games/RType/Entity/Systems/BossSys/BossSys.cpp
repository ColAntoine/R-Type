/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** BossSys system implementation
*/

#include <memory>

#include "BossSys.hpp"

BossSys::BossSys()
: _renderManager(RenderManager::instance())
{
    _bossWeapons[-1] = std::vector<std::string>({"enemy", "bossDrop"});
    _bossWeapons[1] = std::vector<std::string>({"enemy"});
    _bossWeapons[2] = std::vector<std::string>({"enemy"});
    _bossWeapons[3] = std::vector<std::string>({"enemy"});
}

void BossSys::update(registry& r, float dt)
{
    if (shouldSpawn(r, dt)) {
        spawn(r);
    }
    move(r);
}

// TODO: this func should be fied depending on the decision of when it should spawn
// * spawn once for testing
bool BossSys::shouldSpawn(registry &r, float dt)
{
    static bool test = true;
    bool tmp = test;
    test = false;

    return tmp;
}

void BossSys::spawn(registry &r)
{
    auto bossEnt = r.spawn_entity();
    float bossW = _renderManager.scaleSizeW(20);
    float bossH = _renderManager.scaleSizeH(80);

    r.emplace_component<position>(bossEnt, _renderManager.get_screen_infos().getWidth() + 300.f, _renderManager.get_screen_infos().getHeight() / 2.f);
    r.emplace_component<Health>(bossEnt, 1000.f * static_cast<float>(1)); // ! Need to find a way to retrieve the current wave maybe create a wave component
    r.emplace_component<velocity>(bossEnt, -300.f, 0.f);
    r.emplace_component<Boss>(bossEnt);
    r.emplace_component<Enemy>(bossEnt, Enemy::EnemyAIType::BOSS);
    r.emplace_component<collider>(bossEnt, bossW, bossH, -(bossW / 2.f), -(bossH / 2.f));

    // ! the 1 or 1.f is corresponding to the wave used as scale factor for the boss
    Weapon w(
        bossEnt,
        _bossWeapons[-1],
        BOSS_BASE_FIRERATE * 1.f,
        BOSS_BASE_DAMAGE * 1.f,
        BOSS_BASE_PROJ_SPEED * 1.f,
        -1,
        true
    );
    w._wantsToFire = false;
    w._automatic = false;
    r.emplace_component<Weapon>(bossEnt, w);
    // * Spawn the body the arm is not set yet
    r.emplace_component<drawable>(bossEnt, bossW, bossH, 255, 0, 0);


    // ! need sprites for animation
    // r.emplace_component<animation>(bossEnt);
}

void BossSys::move(registry &r)
{
    auto *posArr = r.get_if<position>();
    auto *velArr = r.get_if<velocity>();
    auto *bossArr = r.get_if<Boss>();
    auto *weaponArr = r.get_if<Weapon>();
    static bool doneMoving = false;

    if (!posArr || !velArr || !bossArr || !weaponArr || doneMoving) return;

    for (auto [pos, vel, boss, weap, ent] : zipper(*posArr, *velArr, *bossArr, *weaponArr)) {
        if (pos.x <= _renderManager.scalePosX(75)) {
            vel.vx = 0.f;
            weap._wantsToFire = true;
            weap._automatic = true;
            return;
        }
        vel.vx = -300.f;
    }
}

bool BossSys::isPlayerClose(registry &r)
{

}

void BossSys::smashPlayers(registry &r)
{

}

bool BossSys::isPlayerFar(registry &r)
{

}

bool BossSys::shouldShoot(registry &r)
{

}


extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<BossSys>();
    }
}
