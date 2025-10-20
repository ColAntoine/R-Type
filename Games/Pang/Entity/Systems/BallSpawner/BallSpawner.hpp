/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** BallSpawner system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Zipper.hpp"

#include "Entity/Components/Ball/Ball.hpp"
#include "Constants.hpp"

#include <cstdlib>
#include <ctime>

class BallSpawner : public ISystem {
public:
    BallSpawner() {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }
    
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "BallSpawner"; }
private:
    void getCurrentBalls(registry &r);
    void spawnNewBall(registry &r, float dt);

    // * VARS
    int _currentBalls{0};
    float _spawnTimer{0.f};
    float _spawnInterval{2.f};
    int _minBalls{1};
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
