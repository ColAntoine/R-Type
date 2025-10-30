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
    _bossWeapons[-1] = std::vector<std::string>({"enemy", "bossDrop", "following", "wave"});
    _bossWeapons[1] = std::vector<std::string>({"enemy"});
    _bossWeapons[2] = std::vector<std::string>({"enemy"});
    _bossWeapons[3] = std::vector<std::string>({"enemy", "bossDrop"});
}

void BossSys::update(registry& r, float dt)
{
    if (shouldSpawn(r, dt)) {
        spawn(r);
    }
    move(r);

    bool spe = isPlayerClose(r);
    auto weaponArr = r.get_if<Weapon>();
    auto bossArr = r.get_if<Boss>();

    if (weaponArr && bossArr) {
        for (auto [w, b, ent]: zipper(*weaponArr, *bossArr)) {
            w._shouldShootSpecial = spe;
        }
    }
}

// TODO: this func should be field depending on the decision of when it should spawn
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
    float bossW = static_cast<float>(_renderManager.scaleSizeW(25));
    float bossH = static_cast<float>(_renderManager.scaleSizeH(30));

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

    const float frame_w = 400.0f;
    const float frame_h = 400.0f;
    float scale_x = (frame_w > 0.0f) ? (bossW / frame_w) : 1.0f;
    float scale_y = (frame_h > 0.0f) ? (bossH / frame_h) : 1.0f;
    r.emplace_component<animation>(bossEnt, std::string(RTYPE_PATH_ASSETS) + "bossSheet.png", frame_w, frame_h, scale_x, scale_y, 0, false);
}

void BossSys::move(registry &r)
{
    auto *posArr = r.get_if<position>();
    auto *velArr = r.get_if<velocity>();
    auto *bossArr = r.get_if<Boss>();
    auto *weaponArr = r.get_if<Weapon>();
    static bool doneMoving = false;
    static bool up = false;

    if (!posArr || !velArr || !bossArr || !weaponArr) return;

    if (doneMoving) {
        int screenH = _renderManager.get_screen_infos().getHeight();
        int margin = _renderManager.scaleSizeH(30);

        for (auto [pos, vel, boss, ent] : zipper(*posArr, *velArr, *bossArr)) {
            if (!up) {
                vel.vy = -200.f;
                if (pos.y <= static_cast<float>(margin)) {
                    up = true;
                }
            } else {
                vel.vy = 200.f;
                if (pos.y >= static_cast<float>(screenH - margin)) {
                    up = false;
                }
            }
        }
        return;
    }

    for (auto [pos, vel, boss, weap, ent] : zipper(*posArr, *velArr, *bossArr, *weaponArr)) {
        if (pos.x <= _renderManager.scalePosX(75)) {
            vel.vx = 0.f;
            weap._wantsToFire = true;
            weap._automatic = true;
            doneMoving = true;
            break;
        }
        vel.vx = -300.f;
        return;
    }
}

bool BossSys::isPlayerClose(registry &r)
{
    auto posArr = r.get_if<position>();
    auto playerArr = r.get_if<Player>();
    auto bossArr = r.get_if<Boss>();

    position posPlayer;
    position posBoss;

    if (!posArr || !playerArr || !bossArr) return false;

    for (auto [player, pos, ent] : zipper(*playerArr, *posArr)) {
        posPlayer = pos;
        break;
    }
    for (auto [boss, pos, ent] : zipper(*bossArr, *posArr)) {
        posBoss = pos;
        break;
    }

    float wRange = _renderManager.scaleSizeW(20);

    if (std::abs(posPlayer.x - posBoss.x) <= wRange) {
        return true;
    }
    return false;
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
