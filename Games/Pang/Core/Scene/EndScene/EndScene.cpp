/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Tue, Oct, 2025                                                     *
 * Title           - RTYPE                                                              *
 * Description     -                                                                    *
 *     EndScene                                                                         *
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

#include "EndScene.hpp"
#include "Constants.hpp"
#include "../../../ECS/include/ECS/UI/UIBuilder.hpp"

void EndScene::init(float dt)
{
    _shouldRestart = false;
    _shouldQuit = false;

    _restartButton = ButtonBuilder()
        .centered(50)
        .size(400, 100)
        .text("RESTART")
        .green()
        .textColor(WHITE)
        .fontSize(24)
        .border(2, WHITE)
        .onClick([this]() {
            _shouldRestart = true;
        })
        .build(SCREEN_WIDTH, SCREEN_HEIGHT);

    _quitButton = ButtonBuilder()
        .centered(200)
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

std::optional<GameState> EndScene::update(float dt)
{

    if (_restartButton) {
        _restartButton->handleInput();
        _restartButton->update(dt);
    }
    if (_quitButton) {
        _quitButton->handleInput();
        _quitButton->update(dt);
    }


    if (_shouldRestart) {
        _shouldRestart = false;
        return GameState::INGAME;
    }


    if (_shouldQuit) {
        _shouldQuit = false;
        return GameState::QUIT;
    }

    return std::nullopt;
}

void EndScene::render(float dt)
{
    DrawText("GAME OVER", SCREEN_WIDTH / 2 - 300 + 3, SCREEN_HEIGHT / 2 - 180 + 3, 100, BLACK);
    DrawText("GAME OVER", SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 180, 100, RED);

    DrawText("You died!", SCREEN_WIDTH / 2 - 140 + 2, SCREEN_HEIGHT / 2 - 50 + 2, 50, BLACK);
    DrawText("You died!", SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2 - 50, 50, GRAY);

    if (_restartButton) {
        _restartButton->render();
    }
    if (_quitButton) {
        _quitButton->render();
    }
}

void EndScene::destroy(float dt)
{
    _restartButton.reset();
    _quitButton.reset();
    std::cout << "EndScene destroyed" << std::endl;
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
