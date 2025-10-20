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
#include "Entity/Components/Invincibility/Invincibility.hpp"
#include "Entity/Components/Rope/Rope.hpp"

void RenderSystem::update(registry& r, float dt) {
    renderPlayers(r);
    renderBalls(r);
    renderRopes(r);
}

void RenderSystem::renderPlayers(registry &r)
{
    auto *playerArr = r.get_if<Player>();
    auto *posArr = r.get_if<position>();
    auto *inviArr = r.get_if<Invincibility>();

    if (!playerArr || !posArr) return;

    for (auto&& [player, pos, ent] : zipper(*playerArr, *posArr)) {
        Color playerColor = BLUE;

        if (inviArr && inviArr->has(ent)) {
            auto& invi = inviArr->get(ent);
            if (invi._isInvincible && static_cast<int>(invi._lastActivation * 10) % 2 == 0) {
                playerColor = YELLOW; // Flash yellow when invincible
            }
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

void RenderSystem::renderRopes(registry &r)
{
    auto *ropeArr = r.get_if<Rope>();
    auto *posArr = r.get_if<position>();

    if (!ropeArr || !posArr) return;

    for (auto&& [rope, pos, ent] : zipper(*ropeArr, *posArr)) {
        // Draw rope as a thick vertical line from player position to current rope tip position
        // The rope always goes from the current position to the top (0)
        DrawRectangle(
            static_cast<int>(pos.x - rope._width / 2.0f),  // x position (centered)
            0,                                               // from top of screen
            static_cast<int>(rope._width),                   // width
            static_cast<int>(pos.y),                         // height (extends down to rope position)
            rope._color
        );
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<RenderSystem>();
    }
}
