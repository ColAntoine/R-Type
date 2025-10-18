/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** RenderSystem implementation
*/

#include <memory>
#include <raylib.h>

#include "RenderSystem.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/Zipper.hpp"

#include "Entity/Components/Player/Player.hpp"
#include "Entity/Components/Ball/Ball.hpp"

void RenderSystem::update(registry& r, float dt) {
    renderPlayers(r);
    renderBalls(r);
}

void RenderSystem::renderPlayers(registry &r)
{
    auto *playerArr = r.get_if<Player>();
    auto *posArr = r.get_if<position>();

    if (!playerArr || !posArr) return;

    for (auto&& [player, pos, ent] : zipper(*playerArr, *posArr)) {
        Color playerColor = BLUE;

        if (player._invincibility > 0.0f && static_cast<int>(player._invincibility * 10) % 2 == 0) {
            playerColor = YELLOW;
        }

        DrawRectangle( static_cast<int>(pos.x - 50), static_cast<int>(pos.y - 50), 100, 100, playerColor);
        DrawRectangleLines( static_cast<int>(pos.x - 50), static_cast<int>(pos.y - 50), 100, 100, BLACK);
    }
}

void RenderSystem::renderBalls(registry &r)
{
    auto *ballArr = r.get_if<Ball>();
    auto *posArr = r.get_if<position>();

    if (!ballArr || !posArr) return;

    for (auto&& [ball, pos, ent] : zipper(*ballArr, *posArr)) {
        DrawCircle( static_cast<int>(pos.x), static_cast<int>(pos.y), ball._radius, ball._color);
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<RenderSystem>();
    }
}
