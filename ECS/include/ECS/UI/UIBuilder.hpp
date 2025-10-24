/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Wed, Oct, 2025                                                     *
 * Title           - RTYPE                                                              *
 * Description     -                                                                    *
 *     UIBuilder                                                                        *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *             ███████╗██████╗ ██╗████████╗███████╗ ██████╗██╗  ██╗                     *
 *             ██╔════╝██╔══██╗██║╚══██╔══╝██╔════╝██╔════╝██║  ██║                     *
 *             █████╗  ██████╔╝██║   ██║   █████╗  ██║     ███████║                     *
 *             ██╔══╝  ██╔═══╝ ██║   ██║   ██╔══╝  ██║     ██╔══██║                     *
 *             ███████╗██║     ██║   ██║   ███████╗╚██████╗██║  ██║                     *
 *             ╚══════╝╚═╝     ╚═╝   ╚═╝   ╚══════╝ ╚═════╝╚═╝  ╚═╝                     *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */

#ifndef INCLUDED_UIBUILDER_HPP
    #define INCLUDED_UIBUILDER_HPP

#include "ECS/UI/Components/Button.hpp"
#include "ECS/UI/Components/Text.hpp"
#include "ECS/UI/Components/Panel.hpp"
#include "ECS/UI/Components/InputField.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include <memory>
#include <functional>
#include <algorithm>

class ColorCalculator {
public:
    static Color lighter(Color base, int amount = 20) {
        return Color{
            static_cast<unsigned char>(std::min(255, static_cast<int>(base.r) + amount)),
            static_cast<unsigned char>(std::min(255, static_cast<int>(base.g) + amount)),
            static_cast<unsigned char>(std::min(255, static_cast<int>(base.b) + amount)),
            base.a
        };
    }

    static Color darker(Color base, int amount = 20) {
        return Color{
            static_cast<unsigned char>(std::max(0, static_cast<int>(base.r) - amount)),
            static_cast<unsigned char>(std::max(0, static_cast<int>(base.g) - amount)),
            static_cast<unsigned char>(std::max(0, static_cast<int>(base.b) - amount)),
            base.a
        };
    }
};

template<typename T>
class UIBuilder
{
public:
    UIBuilder() = default;

    UIBuilder& at(float x, float y) {
        _x = x;
        _y = y;
        return *this;
    }

    UIBuilder& centered(float yOffset = 0) {
        _centered = true;
        _centerYOffset = yOffset;
        return *this;
    }

    UIBuilder& size(float width, float height) {
        _width = width;
        _height = height;
        return *this;
    }

    UIBuilder& text(const std::string& text) {
        _text = text;
        return *this;
    }

    UIBuilder& color(Color baseColor, int hoverAmount = 20, int pressAmount = 20) {
        _normalColor = baseColor;
        _hoveredColor = ColorCalculator::lighter(baseColor, hoverAmount);
        _pressedColor = ColorCalculator::darker(baseColor, pressAmount);
        return *this;
    }

    UIBuilder& blue() {
        return color(Color{0, 120, 215, 255});
    }

    UIBuilder& red() {
        return color(Color{180, 0, 0, 255});
    }

    UIBuilder& green() {
        return color(Color{0, 180, 0, 255});
    }

    UIBuilder& yellow() {
        return color(Color{200, 180, 0, 255});
    }

    UIBuilder& gray() {
        return color(Color{100, 100, 100, 255});
    }

    UIBuilder& purple() {
        return color(Color{150, 0, 200, 255});
    }

    UIBuilder& cyan() {
        return color(Color{0, 180, 200, 255});
    }

    UIBuilder& colors(Color normal, Color hovered, Color pressed) {
        _normalColor = normal;
        _hoveredColor = hovered;
        _pressedColor = pressed;
        return *this;
    }

    UIBuilder& textColor(Color color) {
        _textColor = color;
        return *this;
    }

    UIBuilder& fontSize(int size) {
        _fontSize = size;
        return *this;
    }

    UIBuilder& border(float thickness, Color color) {
        _borderThickness = thickness;
        _borderColor = color;
        return *this;
    }

    UIBuilder& onClick(std::function<void()> callback) {
        _onClick = callback;
        return *this;
    }

    UIBuilder& alignment(UI::TextAlignment align) {
        _alignment = align;
        return *this;
    }

    UIBuilder& backgroundColor(Color color) {
        _backgroundColor = color;
        return *this;
    }

    UIBuilder& placeholder(const std::string& text) {
        _placeholderText = text;
        return *this;
    }

    UIBuilder& focusedColor(Color color) {
        _focusedColor = color;
        return *this;
    }

    UIBuilder& focusedBorderColor(Color color) {
        _focusedBorderColor = color;
        return *this;
    }

    UIBuilder& placeholderColor(Color color) {
        _placeholderColor = color;
        return *this;
    }

    UIBuilder& cursorColor(Color color) {
        _cursorColor = color;
        return *this;
    }

    std::shared_ptr<T> build(float screenWidth, float screenHeight);

protected:
    float _x = 0.0f;
    float _y = 0.0f;
    float _width = 400.0f;
    float _height = 100.0f;
    std::string _text = "";
    bool _centered = false;
    float _centerYOffset = 0.0f;

    Color _normalColor = BLUE;
    Color _hoveredColor = SKYBLUE;
    Color _pressedColor = DARKBLUE;
    Color _textColor = WHITE;
    Color _borderColor = WHITE;
    Color _backgroundColor = GRAY;
    Color _focusedColor = LIGHTGRAY;
    Color _focusedBorderColor = SKYBLUE;
    Color _placeholderColor = DARKGRAY;
    Color _cursorColor = WHITE;
    std::string _placeholderText = "";

    int _fontSize = 24;
    float _borderThickness = 2.0f;

    std::function<void()> _onClick = nullptr;
    UI::TextAlignment _alignment = UI::TextAlignment::Left;

    float calculateX(float screenWidth) const {
        if (_centered) {
            return (screenWidth / 2.0f) - (_width / 2.0f);
        }
        return _x;
    }

    float calculateY(float screenHeight) const {
        if (_centered) {
            return (screenHeight / 2.0f) - (_height / 2.0f) + _centerYOffset;
        }
        return _y;
    }
};

// Specializations
template<>
inline std::shared_ptr<UI::UIButton> UIBuilder<UI::UIButton>::build(float screenWidth, float screenHeight) {
    auto button = std::make_shared<UI::UIButton>(
        calculateX(screenWidth), calculateY(screenHeight), _width, _height, _text
    );

    UI::ButtonStyle style;
    style.setNormalColor(_normalColor);
    style.setHoveredColor(_hoveredColor);
    style.setPressedColor(_pressedColor);
    style.setTextColor(_textColor);
    style.setFontSize(_fontSize);
    style.setBorderThickness(_borderThickness);
    style.setBorderColor(_borderColor);
    button->setStyle(style);

    if (_onClick) {
        button->setOnClick(_onClick);
    }

    return button;
}

template<>
inline std::shared_ptr<UI::UIText> UIBuilder<UI::UIText>::build(float screenWidth, float screenHeight) {
    auto &renderManager = RenderManager::instance();
    auto text = std::make_shared<UI::UIText>(
        calculateX(screenWidth), calculateY(screenHeight), _text, _fontSize, _textColor
    );
    if (_centered) {
        Vector2 textSize = MeasureTextEx(renderManager.get_font(), _text.c_str(), static_cast<float>(_fontSize), 0.0f);
        float centeredX = calculateX(screenWidth) + (_width * 0.5f) - (textSize.x * 0.5f);
        float centeredY = calculateY(screenHeight) + (_height * 0.5f) - (textSize.y * 0.5f);
        text->setPosition(centeredX, centeredY);
    } else {
        text->setPosition(calculateX(screenWidth), calculateY(screenHeight));
    }
    text->setAlignment(_alignment);

    return text;
}

template<>
inline std::shared_ptr<UI::UIPanel> UIBuilder<UI::UIPanel>::build(float screenWidth, float screenHeight) {
    auto panel = std::make_shared<UI::UIPanel>(
        calculateX(screenWidth), calculateY(screenHeight), _width, _height
    );

    panel->_style.setBackgroundColor(_backgroundColor);
    panel->_style.setBorderColor(_borderColor);
    panel->_style.setBorderThickness(_borderThickness);

    return panel;
}

template<>
inline std::shared_ptr<UI::UIInputField> UIBuilder<UI::UIInputField>::build(float screenWidth, float screenHeight) {
    auto input = std::make_shared<UI::UIInputField>(
        calculateX(screenWidth), calculateY(screenHeight), _width, _height, _placeholderText
    );

    input->_style.setTextColor(_textColor);
    input->_style.setBackgroundColor(_backgroundColor);
    input->_style.setBorderColor(_borderColor);
    input->_style.setBorderThickness(_borderThickness);
    input->_style.setFocusedColor(_focusedColor);
    input->_style.setFocusedBorderColor(_focusedBorderColor);
    input->_style.setPlaceholderColor(_placeholderColor);
    input->_style.setCursorColor(_cursorColor);
    input->_style.setFontSize(_fontSize);
    return input;
}

// Convenient type aliases
using ButtonBuilder = UIBuilder<UI::UIButton>;
using TextBuilder = UIBuilder<UI::UIText>;
using PanelBuilder = UIBuilder<UI::UIPanel>;
using InputBuilder = UIBuilder<UI::UIInputField>;

#endif

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
