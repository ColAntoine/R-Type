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

void EndScene::init(float dt)
{
    _shouldRestart = false;
    _shouldQuit = false;


    _restartButton = std::make_unique<UI::UIButton>(
        SCREEN_WIDTH / 2.f - 200.f,
        SCREEN_HEIGHT / 2.f + 50.f,
        400.f,
        100.f,
        "RESTART"
    );

    UI::ButtonStyle restartStyle;
    restartStyle._normal_color = Color{0, 180, 0, 255};
    restartStyle._hovered_color = Color{0, 220, 0, 255};
    restartStyle._pressed_color = Color{0, 140, 0, 255};
    restartStyle._text_color = WHITE;
    restartStyle._font_size = 24;
    restartStyle._border_thickness = 2.0f;
    restartStyle._border_color = WHITE;
    _restartButton->set_style(restartStyle);

    _restartButton->set_on_click([this]() {
        _shouldRestart = true;
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

std::optional<GameState> EndScene::update(float dt)
{

    if (_restartButton) {
        _restartButton->handle_input();
        _restartButton->update(dt);
    }
    if (_quitButton) {
        _quitButton->handle_input();
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

    DrawText("GAME OVER", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 120, 60, RED);
    DrawText("You died!", SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2 - 40, 24, GRAY);

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
