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
    _bossWeapons[0] = std::vector<std::string>({});
    _bossWeapons[1] = std::vector<std::string>({"enemy"});
    _bossWeapons[2] = std::vector<std::string>({"enemy, bossDrop"});
    _bossWeapons[3] = std::vector<std::string>({"enemy", "bossDrop", "following"});
    _bossWeapons[4] = std::vector<std::string>({"enemy", "bossDrop", "wave"});
    _bossWeapons[5] = std::vector<std::string>({"enemy", "bossDrop", "following", "wave"});
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
    auto bossArr = r.get_if<Boss>();

    if (!bossArr) return false;

    for (auto [boss, ent]: zipper(*bossArr)) {
        if (boss._shouldSpawn) {
            boss._shouldSpawn = false;
            _wave += 1;
            return true;
        }
        else{
            return false;
        }
    }
    return false;
}

void BossSys::spawn(registry &r)
{
    auto bossArr = r.get_if<Boss>();

    if (!bossArr) return;

    float bossW = static_cast<float>(_renderManager.scaleSizeW(25));
    float bossH = static_cast<float>(_renderManager.scaleSizeH(30));

    for (auto [boss, bossEnt]: zipper(*bossArr)) {
        r.emplace_component<position>(entity(bossEnt), _renderManager.get_screen_infos().getWidth() + 300.f, _renderManager.get_screen_infos().getHeight() / 2.f);
        r.emplace_component<Health>(entity(bossEnt), 1000.f * static_cast<float>(_wave));
        r.emplace_component<velocity>(entity(bossEnt), -300.f, 0.f);
        r.emplace_component<Enemy>(entity(bossEnt), Enemy::EnemyAIType::BOSS);
        r.emplace_component<collider>(entity(bossEnt), bossW, bossH, -(bossW / 2.f), -(bossH / 2.f));
    std::cout << "SPAWNED" << std::endl;

        Weapon w(
            entity(bossEnt),
            _bossWeapons[_wave > 5 ? 5 : _wave],
            BOSS_BASE_FIRERATE * static_cast<float>(_wave),
            BOSS_BASE_DAMAGE * static_cast<float>(_wave),
            BOSS_BASE_PROJ_SPEED * static_cast<float>(_wave),
            -1,
            true
        );
        w._wantsToFire = false;
        w._automatic = false;
        r.emplace_component<Weapon>(entity(bossEnt), w);

        const float frame_w = 400.0f;
        const float frame_h = 400.0f;
        float scale_x = (frame_w > 0.0f) ? (bossW / frame_w) : 1.0f;
        float scale_y = (frame_h > 0.0f) ? (bossH / frame_h) : 1.0f;
        r.emplace_component<animation>(entity(bossEnt), RTYPE_PATH_ASSETS + "bossSheet.png", frame_w, frame_h, scale_x, scale_y, 0, false);
    }

}

void BossSys::move(registry &r)
{
    auto *posArr = r.get_if<position>();
    auto *velArr = r.get_if<velocity>();
    auto *bossArr = r.get_if<Boss>();
    auto *weaponArr = r.get_if<Weapon>();
    static bool up = false;

    if (!posArr || !velArr || !bossArr || !weaponArr) return;

    for (auto [pos, vel, boss, weap, ent] : zipper(*posArr, *velArr, *bossArr, *weaponArr)) {
        if (boss._doneMoving) {
            int screenH = _renderManager.get_screen_infos().getHeight();
            int margin = _renderManager.scaleSizeH(30);

            if (!up) {
                vel.vy = -200.f;
                if (pos.y <= static_cast<float>(margin)) {
                    up = true;
                }
            }
            else {
                vel.vy = 200.f;
                if (pos.y >= static_cast<float>(screenH - margin)) {
                    up = false;
                }
            }
        }
        else {
            if (pos.x <= _renderManager.get_screen_infos().getWidth() * 0.75f) {
                vel.vx = 0.f;
                weap._wantsToFire = true;
                weap._automatic = true;
                boss._doneMoving = true;
            }
            else {
                vel.vx = -300.f;
            }
        }
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

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<BossSys>();
    }
}
