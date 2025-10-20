/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Tue, Oct, 2025                                                     *
 * Title           - RTYPE                                                              *
 * Description     -                                                                    *
 *     InGameScene                                                                      *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *       _|_|_|_|  _|_|_|    _|_|_|  _|_|_|_|_|  _|_|_|_|    _|_|_|  _|    _|           *
 *       _|        _|    _|    _|        _|      _|        _|        _|    _|           *
 *       _|_|_|    _|_|_|      _|        _|      _|_|_|    _|        _|_|_|_|           *
 *       _|        _|          _|        _|      _|        _|        _|    _|           *
 *       _|_|_|_|  _|        _|_|_|      _|      _|_|_|_|    _|_|_|  _|    _|           *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */

#include "InGameScene.hpp"
#include "Constants.hpp"
#include "Entity/Components/Ball/Ball.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Zipper.hpp"

InGameScene::InGameScene(registry& reg, DLLoader& systemLoader, IComponentFactory* factory)
: _reg(reg), _systemLoader(systemLoader), _componentFactory(factory), _initialized(false)
{}

void InGameScene::init(float dt)
{
    Ball ball;
    ball.spawn(_componentFactory, _reg, position(SCREEN_WIDTH / 2.f, 100.f));
    ball.spawn(_componentFactory, _reg, position(SCREEN_WIDTH / 2.f + 30.f, 150.f));

    Player player;
    player.spawn(_componentFactory, _reg, position(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT - 30.f));

    _initialized = true;
}

std::optional<GameState> InGameScene::update(float dt)
{
    _systemLoader.update_all_systems(_reg, dt);

    auto *playerArr = _reg.get_if<Player>();
    if (playerArr) {
        bool hasPlayer = false;
        for (auto [player, ent] : zipper(*playerArr)) {
            hasPlayer = true;
            break;
        }
        if (!hasPlayer) {
            return GameState::END;
        }
    } else {
        return GameState::END;
    }

    return std::nullopt;
}

void InGameScene::render(float dt)
{
    DrawText("Pang Game", 10, 10, 20, RAYWHITE);

    auto *playerArr = _reg.get_if<Player>();
    if (playerArr) {
        for (auto [player, ent] : zipper(*playerArr)) {
            DrawText(TextFormat("Lives: %d", player._life), SCREEN_WIDTH - 120, 10, 20, RAYWHITE);
            break;
        }
    }
}

void InGameScene::destroy(float dt)
{
    auto *playerArr = _reg.get_if<Player>();
    auto *ballArr = _reg.get_if<Ball>();
    std::vector<std::size_t> entitiesToKill;

    if (playerArr) {
        for (auto [player, ent]: zipper(*playerArr)) {
            entitiesToKill.push_back(ent);
        }
    }

    if (ballArr) {
        for (auto [ball, ent]: zipper(*ballArr)) {
            entitiesToKill.push_back(ent);
        }
    }

    for (auto id : entitiesToKill) {
        _reg.kill_entity(entity(id));
    }

    _initialized = false;
}

/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * MIT License                                                                          *
 * Copyright (c) 2025 paul1.emeriau                                                     *
 *                                                                                      *
 * Permission is hereby granted, free of charge, to any person obtaining a copy         *
 * of this software and associated documentation files (the "Software"), to deal        *
 * in the Software without restriction, including without limitation the rights         *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell            *
 * copies of the Software, and to permit persons to whom the Software is                *
 * furnished to do so, subject to the following conditions:                             *
 *                                                                                      *
 * The above copyright notice and this permission notice shall be included in all       *
 * copies or substantial portions of the Software.                                      *
 *                                                                                      *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR           *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,             *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE          *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER               *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,        *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE        *
 * SOFTWARE.                                                                            *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */
