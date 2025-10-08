/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI Text Component
*/

#pragma once

#include "../UIComponent.hpp"
#include <string>

enum class TextAlignment {
    Left,
    Center,
    Right
};

class UIText : public UIComponent {
private:
    std::string text_;
    Color text_color_{255, 255, 255, 255}; // White by default
    int font_size_{20};
    TextAlignment alignment_{TextAlignment::Left};
    bool auto_size_{true}; // Automatically adjust size to fit text

public:
    UIText() = default;
    UIText(float x, float y, const std::string& text, int font_size = 20)
        : UIComponent(x, y, 0, 0), text_(text), font_size_(font_size) {
        update_size();
    }

    // IUIComponent implementation
    void update(float delta_time) override;
    void render() override;
    void handle_input() override; // Text doesn't handle input by default

    // Text-specific methods
    void set_text(const std::string& text) { 
        text_ = text; 
        if (auto_size_) update_size();
    }
    void set_text_color(Color color) { text_color_ = color; }
    void set_font_size(int size) { 
        font_size_ = size; 
        if (auto_size_) update_size();
    }
    void set_alignment(TextAlignment alignment) { alignment_ = alignment; }
    void set_auto_size(bool auto_size) { 
        auto_size_ = auto_size; 
        if (auto_size_) update_size();
    }

    const std::string& get_text() const { return text_; }
    int get_font_size() const { return font_size_; }
    TextAlignment get_alignment() const { return alignment_; }

private:
    void update_size();
    Vector2 get_text_position() const;
};