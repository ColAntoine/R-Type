/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PowerUpSys system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include "ECS/Registry.hpp"

#include "ECS/Components/Collider.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"

#include "Entity/Components/PowerUp/PowerUp.hpp"
#include "Entity/Components/Drawable/Drawable.hpp"
#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Components/Player/Player.hpp"

#include <map>
#include <functional>
#include <random>

class PowerUpSys : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "PowerUpSys"; }

    PowerUpSys();

private:
    void spawnPowerUps(registry &r, float dt);
    void colisionPowerUps(registry &r, float dt);
    void applyPowerUps(Weapon &weapon, velocity *vel, PowerUp &pUp);

    std::map<powerUpType, std::function<void(registry &)>> _pFunc;

    std::mt19937 rng_;
    std::uniform_real_distribution<> x_dist_;
    std::uniform_real_distribution<> y_dist_;
    std::uniform_int_distribution<> powerup_type_dist_;
    float spawn_timer_{0.0f};
    float spawn_interval_{10.0f};
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
