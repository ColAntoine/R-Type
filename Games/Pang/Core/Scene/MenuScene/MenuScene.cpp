/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Tue, Oct, 2025                                                     *
 * Title           - RTYPE                                                              *
 * Description     -                                                                    *
 *     MenuScene                                                                        *
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

#include "MenuScene.hpp"
#include "Constants.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include <optional>

void MenuScene::init(float dt)
{
    _shouldStartGame = false;
    _shouldQuit = false;

    // Create PLAY button using builder
    _playButton = ButtonBuilder()
        .centered(200)
        .size(400, 100)
        .text("PLAY")
        .blue()
        .textColor(WHITE)
        .fontSize(24)
        .border(2, WHITE)
        .onClick([this]() {
            _shouldStartGame = true;
        })
        .build(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Create QUIT button using builder
    _quitButton = ButtonBuilder()
        .centered(350)
        .size(400, 100)
        .text("QUIT")
        .red()
        .textColor(WHITE)
        .fontSize(24)
        .border(2, WHITE)
        .onClick([this]() {
            _shouldQuit = true;
        })
        .build(SCREEN_WIDTH, SCREEN_HEIGHT);
}

std::optional<GameState> MenuScene::update(float dt)
{

    if (_playButton) {
        _playButton->handleInput();
        _playButton->update(dt);
    }
    if (_quitButton) {
        _quitButton->handleInput();
        _quitButton->update(dt);
    }


    if (_shouldStartGame) {
        _shouldStartGame = false;
        return GameState::INGAME;
    }

    if (_shouldQuit) {
        _shouldQuit = false;
        return GameState::QUIT;
    }

    return std::nullopt;
}

void MenuScene::render(float dt)
{
    DrawText("PANG", SCREEN_WIDTH / 2 - 150 + 3, SCREEN_HEIGHT / 2 - 200 + 3, 120, BLACK);
    DrawText("PANG", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 200, 120, RAYWHITE);

    DrawText("Use Arrow Keys to Move", SCREEN_WIDTH / 2 - 250 + 2, SCREEN_HEIGHT / 2 - 50 + 2, 40, BLACK);
    DrawText("Use Arrow Keys to Move", SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 50, 40, GRAY);

    DrawText("Press SPACE to Shoot!", SCREEN_WIDTH / 2 - 220 + 2, SCREEN_HEIGHT / 2 - 5 + 2, 40, BLACK);
    DrawText("Press SPACE to Shoot!", SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 - 5, 40, GRAY);

    if (_playButton) {
        _playButton->render();
    }
    if (_quitButton) {
        _quitButton->render();
    }
}

void MenuScene::destroy(float dt)
{
    _playButton.reset();
    _quitButton.reset();
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
