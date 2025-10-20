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
#include <optional>

void MenuScene::init(float dt)
{
    _shouldStartGame = false;
    _shouldQuit = false;

    _playButton = std::make_unique<UI::UIButton>(
        SCREEN_WIDTH / 2.f - 200.f,
        SCREEN_HEIGHT / 2.f + 50.f,
        400.f,
        100.f,
        "PLAY"
    );

    UI::ButtonStyle playStyle;
    playStyle._normal_color = Color{0, 120, 215, 255};
    playStyle._hovered_color = Color{0, 150, 255, 255};
    playStyle._pressed_color = Color{0, 90, 180, 255};
    playStyle._text_color = WHITE;
    playStyle._font_size = 24;
    playStyle._border_thickness = 2.0f;
    playStyle._border_color = WHITE;
    _playButton->set_style(playStyle);

    _playButton->set_on_click([this]() {
        _shouldStartGame = true;
    });

    _quitButton = std::make_unique<UI::UIButton>(
        SCREEN_WIDTH / 2.f - 200.f,
        SCREEN_HEIGHT / 2.f + 200.f,
        400.f,
        100.f,
        "QUIT"
    );

    UI::ButtonStyle quitStyle;
    quitStyle._normal_color = Color{180, 0, 0, 255};
    quitStyle._hovered_color = Color{220, 0, 0, 255};
    quitStyle._pressed_color = Color{140, 0, 0, 255};
    quitStyle._text_color = WHITE;
    quitStyle._font_size = 24;
    quitStyle._border_thickness = 2.0f;
    quitStyle._border_color = WHITE;
    _quitButton->set_style(quitStyle);


    _quitButton->set_on_click([this]() {
        _shouldQuit = true;
    });
}

std::optional<GameState> MenuScene::update(float dt)
{

    if (_playButton) {
        _playButton->handle_input();
        _playButton->update(dt);
    }
    if (_quitButton) {
        _quitButton->handle_input();
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
    DrawText("PANG", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 200, 120, RAYWHITE);
    DrawText("Use Arrow Keys to Move", SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 50, 40, GRAY);
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
