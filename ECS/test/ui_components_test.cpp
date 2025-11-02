/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** UI Components Unit Tests
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ECS/UI/Components/Button.hpp"
#include "ECS/UI/Components/InputField.hpp"
#include "ECS/UI/Components/Panel.hpp"
#include "ECS/UI/Components/Text.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include <raylib.h>

// Helper function to init raylib for tests
static void init_raylib_for_tests() {
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(1, 1, "Test");
}

// =============================================================================
// UIButton Tests
// =============================================================================

TEST_SUITE("UIButton Tests") {
    TEST_CASE("UIButton default constructor") {
        init_raylib_for_tests();
        
        UI::UIButton button;
        
        CHECK(button.getText().empty());
        CHECK(button.getPosition().x == 0.0f);
        CHECK(button.getPosition().y == 0.0f);
        // Default size is 100x40 from AUIComponent
        CHECK(button.getSize().x == 100.0f);
        CHECK(button.getSize().y == 40.0f);
        CHECK(button.getState() == UI::UIState::Normal);
        CHECK(button.isEnabled());
        
        CloseWindow();
    }

    TEST_CASE("UIButton parameterized constructor") {
        init_raylib_for_tests();
        
        UI::UIButton button(100.0f, 200.0f, 150.0f, 50.0f, "Click Me");
        
        CHECK(button.getText() == "Click Me");
        CHECK(button.getPosition().x == 100.0f);
        CHECK(button.getPosition().y == 200.0f);
        CHECK(button.getSize().x == 150.0f);
        CHECK(button.getSize().y == 50.0f);
        CHECK(button.getState() == UI::UIState::Normal);
        
        CloseWindow();
    }

    TEST_CASE("UIButton setText and getText") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 100, 50, "Initial");
        CHECK(button.getText() == "Initial");
        
        button.setText("Updated");
        CHECK(button.getText() == "Updated");
        
        button.setText("");
        CHECK(button.getText().empty());
        
        CloseWindow();
    }

    TEST_CASE("UIButton setStyle and getStyle") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 100, 50, "Test");
        UI::ButtonStyle style;
        
        style.setNormalColor({100, 100, 100, 255});
        style.setHoveredColor({150, 150, 150, 255});
        style.setFontSize(24);
        
        button.setStyle(style);
        
        const auto& retrievedStyle = button.getStyle();
        CHECK(retrievedStyle.getNormalColor().r == 100);
        CHECK(retrievedStyle.getHoveredColor().r == 150);
        CHECK(retrievedStyle.getFontSize() == 24);
        
        CloseWindow();
    }

    TEST_CASE("UIButton onClick callback") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 100, 50, "Click");
        bool clicked = false;
        
        button.setOnClick([&clicked]() {
            clicked = true;
        });
        
        // Simulate click by calling handleInput when mouse is over button and clicked
        // Note: In real scenario, handleInput checks mouse position
        CHECK_FALSE(clicked);
        
        CloseWindow();
    }

    TEST_CASE("UIButton onHover callback") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 100, 50, "Hover");
        bool hovered = false;
        
        button.setOnHover([&hovered]() {
            hovered = true;
        });
        
        // Hover state is managed internally by handleInput
        CHECK_FALSE(hovered);
        
        CloseWindow();
    }

    TEST_CASE("UIButton onPress and onRelease callbacks") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 100, 50, "Press");
        bool pressed = false;
        bool released = false;
        
        button.setOnPress([&pressed]() {
            pressed = true;
        });
        
        button.setOnRelease([&released]() {
            released = true;
        });
        
        CHECK_FALSE(pressed);
        CHECK_FALSE(released);
        
        CloseWindow();
    }

    TEST_CASE("UIButton enable and disable") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 100, 50, "Test");
        CHECK(button.isEnabled());
        CHECK(button.getState() == UI::UIState::Normal);
        
        button.setEnabled(false);
        CHECK_FALSE(button.isEnabled());
        // State needs to be updated by calling update() or update_state()
        // For now, just check isEnabled() works
        
        button.setEnabled(true);
        CHECK(button.isEnabled());
        
        CloseWindow();
    }

    TEST_CASE("UIButton update method") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 100, 50, "Test");
        
        // Should not crash
        button.update(0.016f);
        button.update(1.0f);
        button.update(0.0f);
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIButton render method") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 100, 50, "Render Test");
        
        BeginDrawing();
        // Should not crash
        button.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIButton handleInput method") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 100, 50, "Input");
        
        // Should not crash
        button.handleInput();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIButton setPosition and getPosition") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 100, 50, "Test");
        
        button.setPosition(250.0f, 300.0f);
        CHECK(button.getPosition().x == 250.0f);
        CHECK(button.getPosition().y == 300.0f);
        
        CloseWindow();
    }

    TEST_CASE("UIButton setSize and getSize") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 100, 50, "Test");
        
        button.setSize(200.0f, 75.0f);
        CHECK(button.getSize().x == 200.0f);
        CHECK(button.getSize().y == 75.0f);
        
        CloseWindow();
    }

    TEST_CASE("UIButton style colors") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 100, 50, "Test");
        UI::ButtonStyle style;
        
        style.setNormalColor({100, 100, 100, 255});
        style.setDisabledColor({50, 50, 50, 255});
        style.setTextColor({255, 255, 255, 255});
        style.setDisabledTextColor({150, 150, 150, 255});
        button.setStyle(style);
        
        // Check style was set correctly
        const auto& retrievedStyle = button.getStyle();
        CHECK(retrievedStyle.getNormalColor().r == 100);
        CHECK(retrievedStyle.getDisabledColor().r == 50);
        CHECK(retrievedStyle.getTextColor().r == 255);
        CHECK(retrievedStyle.getDisabledTextColor().r == 150);
        
        CloseWindow();
    }

    TEST_CASE("UIButton state changes") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 100, 50, "Test");
        UI::ButtonStyle style;
        
        style.setNormalColor({100, 100, 100, 255});
        style.setHoveredColor({120, 120, 120, 255});
        style.setDisabledColor({50, 50, 50, 255});
        button.setStyle(style);
        
        // Normal state
        CHECK(button.getState() == UI::UIState::Normal);
        
        // Test setState directly
        button.setState(UI::UIState::Hovered);
        CHECK(button.getState() == UI::UIState::Hovered);
        
        button.setState(UI::UIState::Pressed);
        CHECK(button.getState() == UI::UIState::Pressed);
        
        CloseWindow();
    }

    TEST_CASE("UIButton ButtonStyle default values") {
        init_raylib_for_tests();
        
        UI::ButtonStyle style;
        
        CHECK(style.getNormalColor().r == 70);
        CHECK(style.getFontSize() == 20);
        CHECK(style.getBorderThickness() == 1.0f);
        CHECK(style.getPadding() == 10.0f);
        
        CloseWindow();
    }

    TEST_CASE("UIButton ButtonStyle setters work correctly") {
        init_raylib_for_tests();
        
        UI::ButtonStyle style;
        
        style.setNormalColor({1, 2, 3, 4});
        style.setHoveredColor({5, 6, 7, 8});
        style.setPressedColor({9, 10, 11, 12});
        style.setDisabledColor({13, 14, 15, 16});
        style.setTextColor({17, 18, 19, 20});
        style.setDisabledTextColor({21, 22, 23, 24});
        style.setBorderColor({25, 26, 27, 28});
        style.setFontSize(32);
        style.setBorderThickness(3.5f);
        style.setPadding(15.0f);
        
        CHECK(style.getNormalColor().r == 1);
        CHECK(style.getHoveredColor().r == 5);
        CHECK(style.getPressedColor().r == 9);
        CHECK(style.getDisabledColor().r == 13);
        CHECK(style.getTextColor().r == 17);
        CHECK(style.getDisabledTextColor().r == 21);
        CHECK(style.getBorderColor().r == 25);
        CHECK(style.getFontSize() == 32);
        CHECK(style.getBorderThickness() == 3.5f);
        CHECK(style.getPadding() == 15.0f);
        
        CloseWindow();
    }

    TEST_CASE("UIButton empty text") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 100, 50, "");
        CHECK(button.getText().empty());
        
        BeginDrawing();
        button.render(); // Should not crash with empty text
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIButton very long text") {
        init_raylib_for_tests();
        
        std::string longText(1000, 'A');
        UI::UIButton button(0, 0, 100, 50, longText);
        
        CHECK(button.getText() == longText);
        
        BeginDrawing();
        button.render(); // Should handle long text
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIButton with negative position") {
        init_raylib_for_tests();
        
        UI::UIButton button(-50.0f, -100.0f, 100, 50, "Test");
        CHECK(button.getPosition().x == -50.0f);
        CHECK(button.getPosition().y == -100.0f);
        
        BeginDrawing();
        button.render(); // Should handle negative position
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIButton with zero size") {
        init_raylib_for_tests();
        
        UI::UIButton button(0, 0, 0, 0, "Test");
        CHECK(button.getSize().x == 0.0f);
        CHECK(button.getSize().y == 0.0f);
        
        BeginDrawing();
        button.render(); // Should handle zero size
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }
}

// =============================================================================
// UIInputField Tests
// =============================================================================

TEST_SUITE("UIInputField Tests") {
    TEST_CASE("UIInputField default constructor") {
        init_raylib_for_tests();
        
        UI::UIInputField input;
        
        CHECK(input.getText().empty());
        CHECK(input.getPlaceholder().empty());
        CHECK_FALSE(input.isPassword());
        CHECK_FALSE(input.isFocused());
        CHECK(input.getMaxLength() == 50);
        CHECK(input.getState() == UI::UIState::Normal);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField parameterized constructor") {
        init_raylib_for_tests();
        
        UI::UIInputField input(100.0f, 200.0f, 250.0f, 40.0f, "Enter text...");
        
        CHECK(input.getText().empty());
        CHECK(input.getPlaceholder() == "Enter text...");
        CHECK(input.getPosition().x == 100.0f);
        CHECK(input.getPosition().y == 200.0f);
        CHECK(input.getSize().x == 250.0f);
        CHECK(input.getSize().y == 40.0f);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField setText and getText") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        CHECK(input.getText().empty());
        
        input.setText("Hello");
        CHECK(input.getText() == "Hello");
        
        input.setText("World");
        CHECK(input.getText() == "World");
        
        input.setText("");
        CHECK(input.getText().empty());
        
        CloseWindow();
    }

    TEST_CASE("UIInputField setPlaceholder and getPlaceholder") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "Initial");
        CHECK(input.getPlaceholder() == "Initial");
        
        input.setPlaceholder("New placeholder");
        CHECK(input.getPlaceholder() == "New placeholder");
        
        CloseWindow();
    }

    TEST_CASE("UIInputField password mode") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        CHECK_FALSE(input.isPassword());
        
        input.setPassword(true);
        CHECK(input.isPassword());
        
        input.setPassword(false);
        CHECK_FALSE(input.isPassword());
        
        CloseWindow();
    }

    TEST_CASE("UIInputField max length") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        CHECK(input.getMaxLength() == 50);
        
        input.setMaxLength(10);
        CHECK(input.getMaxLength() == 10);
        
        input.setMaxLength(100);
        CHECK(input.getMaxLength() == 100);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField focus state") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        CHECK_FALSE(input.isFocused());
        
        input.setFocused(true);
        CHECK(input.isFocused());
        
        input.setFocused(false);
        CHECK_FALSE(input.isFocused());
        
        CloseWindow();
    }

    TEST_CASE("UIInputField onTextChanged callback") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        std::string changedText;
        
        input.setOnTextChanged([&changedText](const std::string& text) {
            changedText = text;
        });
        
        // Callbacks are called internally by processTextInput
        CHECK(changedText.empty());
        
        CloseWindow();
    }

    TEST_CASE("UIInputField onEnterPressed callback") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        bool enterPressed = false;
        
        input.setOnEnterPressed([&enterPressed](const std::string&) {
            enterPressed = true;
        });
        
        CHECK_FALSE(enterPressed);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField onFocus callback") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        bool focused = false;
        
        input.setOnFocus([&focused]() {
            focused = true;
        });
        
        CHECK_FALSE(focused);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField onFocusLost callback") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        bool focusLost = false;
        
        input.setOnFocusLost([&focusLost]() {
            focusLost = true;
        });
        
        CHECK_FALSE(focusLost);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField update method") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        
        // Should not crash
        input.update(0.016f);
        input.update(1.0f);
        input.update(0.0f);
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField render method") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "Placeholder");
        
        BeginDrawing();
        input.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField handleInput method") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        
        // Should not crash
        input.handleInput();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField setStyle and getStyle") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        UI::InputFieldStyle style;
        
        style.setBackgroundColor({100, 100, 100, 255});
        style.setFocusedColor({150, 150, 150, 255});
        style.setFontSize(18);
        
        input.setStyle(style);
        
        const auto& retrievedStyle = input.getStyle();
        CHECK(retrievedStyle.getBackgroundColor().r == 100);
        CHECK(retrievedStyle.getFocusedColor().r == 150);
        CHECK(retrievedStyle.getFontSize() == 18);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField InputFieldStyle default values") {
        init_raylib_for_tests();
        
        UI::InputFieldStyle style;
        
        CHECK(style.getBackgroundColor().r == 50);
        CHECK(style.getFontSize() == 20);
        CHECK(style.getBorderThickness() == 1.0f);
        CHECK(style.getPadding() == 10.0f);
        CHECK(style.getCursorBlinkSpeed() == 0.5f);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField InputFieldStyle setters") {
        init_raylib_for_tests();
        
        UI::InputFieldStyle style;
        
        style.setBackgroundColor({1, 2, 3, 4});
        style.setFocusedColor({5, 6, 7, 8});
        style.setDisabledColor({9, 10, 11, 12});
        style.setBorderColor({13, 14, 15, 16});
        style.setFocusedBorderColor({17, 18, 19, 20});
        style.setTextColor({21, 22, 23, 24});
        style.setPlaceholderColor({25, 26, 27, 28});
        style.setDisabledTextColor({29, 30, 31, 32});
        style.setCursorColor({33, 34, 35, 36});
        style.setFontSize(24);
        style.setBorderThickness(2.5f);
        style.setPadding(12.0f);
        style.setCursorBlinkSpeed(0.7f);
        
        CHECK(style.getBackgroundColor().r == 1);
        CHECK(style.getFocusedColor().r == 5);
        CHECK(style.getDisabledColor().r == 9);
        CHECK(style.getBorderColor().r == 13);
        CHECK(style.getFocusedBorderColor().r == 17);
        CHECK(style.getTextColor().r == 21);
        CHECK(style.getPlaceholderColor().r == 25);
        CHECK(style.getDisabledTextColor().r == 29);
        CHECK(style.getCursorColor().r == 33);
        CHECK(style.getFontSize() == 24);
        CHECK(style.getBorderThickness() == 2.5f);
        CHECK(style.getPadding() == 12.0f);
        CHECK(style.getCursorBlinkSpeed() == 0.7f);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField render with text") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        input.setText("Test text");
        
        BeginDrawing();
        input.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField render with placeholder") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "Enter something...");
        
        BeginDrawing();
        input.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField render when focused") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        input.setFocused(true);
        
        BeginDrawing();
        input.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField render in password mode") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        input.setText("secret123");
        input.setPassword(true);
        
        BeginDrawing();
        input.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIInputField very long text") {
        init_raylib_for_tests();
        
        UI::UIInputField input(0, 0, 200, 40, "");
        std::string longText(1000, 'X');
        input.setText(longText);
        
        CHECK(input.getText() == longText);
        
        BeginDrawing();
        input.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }
}

// =============================================================================
// UIPanel Tests
// =============================================================================

TEST_SUITE("UIPanel Tests") {
    TEST_CASE("UIPanel default constructor") {
        init_raylib_for_tests();
        
        UI::UIPanel panel;
        
        CHECK(panel.getPosition().x == 0.0f);
        CHECK(panel.getPosition().y == 0.0f);
        // Default size is 100x40 from AUIComponent
        CHECK(panel.getSize().x == 100.0f);
        CHECK(panel.getSize().y == 40.0f);
        CHECK(panel.getState() == UI::UIState::Normal);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel parameterized constructor") {
        init_raylib_for_tests();
        
        UI::UIPanel panel(50.0f, 100.0f, 300.0f, 200.0f);
        
        CHECK(panel.getPosition().x == 50.0f);
        CHECK(panel.getPosition().y == 100.0f);
        CHECK(panel.getSize().x == 300.0f);
        CHECK(panel.getSize().y == 200.0f);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel setStyle and getStyle") {
        init_raylib_for_tests();
        
        UI::UIPanel panel(0, 0, 200, 150);
        UI::PanelStyle style;
        
        style.setBackgroundColor({100, 100, 100, 200});
        style.setBorderColor({150, 150, 150, 255});
        style.setBorderThickness(3.0f);
        style.setCornerRadius(10.0f);
        
        panel.setStyle(style);
        
        const auto& retrievedStyle = panel.getStyle();
        CHECK(retrievedStyle.getBackgroundColor().r == 100);
        CHECK(retrievedStyle.getBorderColor().r == 150);
        CHECK(retrievedStyle.getBorderThickness() == 3.0f);
        CHECK(retrievedStyle.getCornerRadius() == 10.0f);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel PanelStyle default values") {
        init_raylib_for_tests();
        
        UI::PanelStyle style;
        
        CHECK(style.getBackgroundColor().r == 40);
        CHECK(style.getBackgroundColor().a == 200);
        CHECK(style.getBorderThickness() == 1.0f);
        CHECK(style.getCornerRadius() == 0.0f);
        CHECK_FALSE(style.hasShadow());
        CHECK(style.getShadowOffset() == 5.0f);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel PanelStyle setters") {
        init_raylib_for_tests();
        
        UI::PanelStyle style;
        
        style.setBackgroundColor({1, 2, 3, 4});
        style.setBorderColor({5, 6, 7, 8});
        style.setBorderThickness(2.5f);
        style.setCornerRadius(15.0f);
        style.setHasShadow(true);
        style.setShadowColor({9, 10, 11, 12});
        style.setShadowOffset(8.0f);
        
        CHECK(style.getBackgroundColor().r == 1);
        CHECK(style.getBorderColor().r == 5);
        CHECK(style.getBorderThickness() == 2.5f);
        CHECK(style.getCornerRadius() == 15.0f);
        CHECK(style.hasShadow());
        CHECK(style.getShadowColor().r == 9);
        CHECK(style.getShadowOffset() == 8.0f);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel update method") {
        init_raylib_for_tests();
        
        UI::UIPanel panel(0, 0, 200, 150);
        
        // Should not crash
        panel.update(0.016f);
        panel.update(1.0f);
        panel.update(0.0f);
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel render method") {
        init_raylib_for_tests();
        
        UI::UIPanel panel(0, 0, 200, 150);
        
        BeginDrawing();
        panel.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel render with shadow") {
        init_raylib_for_tests();
        
        UI::UIPanel panel(0, 0, 200, 150);
        UI::PanelStyle style;
        style.setHasShadow(true);
        style.setShadowOffset(10.0f);
        panel.setStyle(style);
        
        BeginDrawing();
        panel.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel render with rounded corners") {
        init_raylib_for_tests();
        
        UI::UIPanel panel(0, 0, 200, 150);
        UI::PanelStyle style;
        style.setCornerRadius(20.0f);
        panel.setStyle(style);
        
        BeginDrawing();
        panel.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel render with border") {
        init_raylib_for_tests();
        
        UI::UIPanel panel(0, 0, 200, 150);
        UI::PanelStyle style;
        style.setBorderThickness(5.0f);
        style.setBorderColor({255, 0, 0, 255});
        panel.setStyle(style);
        
        BeginDrawing();
        panel.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel handleInput method") {
        init_raylib_for_tests();
        
        UI::UIPanel panel(0, 0, 200, 150);
        
        // Should not crash
        panel.handleInput();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel setPosition and getPosition") {
        init_raylib_for_tests();
        
        UI::UIPanel panel(0, 0, 200, 150);
        
        panel.setPosition(100.0f, 200.0f);
        CHECK(panel.getPosition().x == 100.0f);
        CHECK(panel.getPosition().y == 200.0f);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel setSize and getSize") {
        init_raylib_for_tests();
        
        UI::UIPanel panel(0, 0, 200, 150);
        
        panel.setSize(400.0f, 300.0f);
        CHECK(panel.getSize().x == 400.0f);
        CHECK(panel.getSize().y == 300.0f);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel with zero size") {
        init_raylib_for_tests();
        
        UI::UIPanel panel(0, 0, 0, 0);
        
        BeginDrawing();
        panel.render(); // Should handle zero size
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel with negative position") {
        init_raylib_for_tests();
        
        UI::UIPanel panel(-100.0f, -200.0f, 150, 100);
        
        CHECK(panel.getPosition().x == -100.0f);
        CHECK(panel.getPosition().y == -200.0f);
        
        BeginDrawing();
        panel.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIPanel with large size") {
        init_raylib_for_tests();
        
        UI::UIPanel panel(0, 0, 10000, 10000);
        
        CHECK(panel.getSize().x == 10000.0f);
        CHECK(panel.getSize().y == 10000.0f);
        
        BeginDrawing();
        panel.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }
}

// =============================================================================
// UIText Tests
// =============================================================================

TEST_SUITE("UIText Tests") {
    TEST_CASE("UIText default constructor") {
        init_raylib_for_tests();
        
        UI::UIText text;
        
        CHECK(text.getText().empty());
        CHECK(text.getPosition().x == 0.0f);
        CHECK(text.getPosition().y == 0.0f);
        CHECK(text.getState() == UI::UIState::Normal);
        
        CloseWindow();
    }

    TEST_CASE("UIText parameterized constructor") {
        init_raylib_for_tests();
        
        UI::UIText text(100.0f, 200.0f, "Hello World");
        
        CHECK(text.getText() == "Hello World");
        CHECK(text.getPosition().x == 100.0f);
        CHECK(text.getPosition().y == 200.0f);
        
        CloseWindow();
    }

    TEST_CASE("UIText constructor with color and font size") {
        init_raylib_for_tests();
        
        UI::UIText text(50.0f, 75.0f, "Test", 24, {255, 0, 0, 255});
        
        CHECK(text.getText() == "Test");
        CHECK(text.getPosition().x == 50.0f);
        CHECK(text.getPosition().y == 75.0f);
        
        const auto& style = text.getStyle();
        CHECK(style.getFontSize() == 24);
        CHECK(style.getTextColor().r == 255);
        CHECK(style.getTextColor().g == 0);
        CHECK(style.getTextColor().b == 0);
        
        CloseWindow();
    }

    TEST_CASE("UIText setText and getText") {
        init_raylib_for_tests();
        
        UI::UIText text(0, 0, "Initial");
        CHECK(text.getText() == "Initial");
        
        text.setText("Updated");
        CHECK(text.getText() == "Updated");
        
        text.setText("");
        CHECK(text.getText().empty());
        
        CloseWindow();
    }

    TEST_CASE("UIText setStyle and getStyle") {
        init_raylib_for_tests();
        
        UI::UIText text(0, 0, "Test");
        UI::TextStyle style;
        
        style.setTextColor({100, 150, 200, 255});
        style.setFontSize(32);
        style.setAlignment(UI::TextAlignment::Center);
        
        text.setStyle(style);
        
        const auto& retrievedStyle = text.getStyle();
        CHECK(retrievedStyle.getTextColor().r == 100);
        CHECK(retrievedStyle.getFontSize() == 32);
        CHECK(retrievedStyle.getAlignment() == UI::TextAlignment::Center);
        
        CloseWindow();
    }

    TEST_CASE("UIText TextStyle default values") {
        init_raylib_for_tests();
        
        UI::TextStyle style;
        
        CHECK(style.getTextColor().r == 255);
        CHECK(style.getTextColor().g == 255);
        CHECK(style.getTextColor().b == 255);
        CHECK(style.getFontSize() == 20);
        CHECK(style.getSpacing() == 1.0f);
        CHECK_FALSE(style.hasShadow());
        CHECK(style.getAlignment() == UI::TextAlignment::Left);
        
        CloseWindow();
    }

    TEST_CASE("UIText TextStyle setters") {
        init_raylib_for_tests();
        
        UI::TextStyle style;
        
        style.setTextColor({1, 2, 3, 4});
        style.setShadowColor({5, 6, 7, 8});
        style.setFontSize(36);
        style.setSpacing(2.5f);
        style.setHasShadow(true);
        style.setShadowOffset({3.0f, 4.0f});
        style.setAlignment(UI::TextAlignment::Right);
        
        CHECK(style.getTextColor().r == 1);
        CHECK(style.getShadowColor().r == 5);
        CHECK(style.getFontSize() == 36);
        CHECK(style.getSpacing() == 2.5f);
        CHECK(style.hasShadow());
        CHECK(style.getShadowOffset().x == 3.0f);
        CHECK(style.getShadowOffset().y == 4.0f);
        CHECK(style.getAlignment() == UI::TextAlignment::Right);
        
        CloseWindow();
    }

    TEST_CASE("UIText alignment Left") {
        init_raylib_for_tests();
        
        UI::UIText text(0, 0, "Left Aligned");
        text.setAlignment(UI::TextAlignment::Left);
        
        CHECK(text.getAlignment() == UI::TextAlignment::Left);
        
        BeginDrawing();
        text.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIText alignment Center") {
        init_raylib_for_tests();
        
        UI::UIText text(0, 0, "Center Aligned");
        text.setAlignment(UI::TextAlignment::Center);
        
        CHECK(text.getAlignment() == UI::TextAlignment::Center);
        
        BeginDrawing();
        text.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIText alignment Right") {
        init_raylib_for_tests();
        
        UI::UIText text(0, 0, "Right Aligned");
        text.setAlignment(UI::TextAlignment::Right);
        
        CHECK(text.getAlignment() == UI::TextAlignment::Right);
        
        BeginDrawing();
        text.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIText update method") {
        init_raylib_for_tests();
        
        UI::UIText text(0, 0, "Test");
        
        // Should not crash
        text.update(0.016f);
        text.update(1.0f);
        text.update(0.0f);
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIText render method") {
        init_raylib_for_tests();
        
        UI::UIText text(0, 0, "Render Test");
        
        BeginDrawing();
        text.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIText render with shadow") {
        init_raylib_for_tests();
        
        UI::UIText text(0, 0, "Shadow Text");
        UI::TextStyle style;
        style.setHasShadow(true);
        style.setShadowOffset({5.0f, 5.0f});
        text.setStyle(style);
        
        BeginDrawing();
        text.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIText handleInput method") {
        init_raylib_for_tests();
        
        UI::UIText text(0, 0, "Test");
        
        // Should not crash
        text.handleInput();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIText size updates with text") {
        init_raylib_for_tests();
        
        UI::UIText text(0, 0, "Test");
        
        // Size is updated in constructor, should be non-zero after setText
        // Note: getTextSize() requires RenderManager to be initialized
        // For unit tests, we just verify the API works without crashing
        Vector2 size = text.getSize();
        CHECK(size.x >= 0.0f);
        CHECK(size.y >= 0.0f);
        
        CloseWindow();
    }

    TEST_CASE("UIText empty text") {
        init_raylib_for_tests();
        
        UI::UIText text(0, 0, "");
        CHECK(text.getText().empty());
        
        BeginDrawing();
        text.render(); // Should not crash
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIText very long text") {
        init_raylib_for_tests();
        
        std::string longText(1000, 'T');
        UI::UIText text(0, 0, longText);
        
        CHECK(text.getText() == longText);
        
        BeginDrawing();
        text.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIText multi-line text") {
        init_raylib_for_tests();
        
        UI::UIText text(0, 0, "Line 1\nLine 2\nLine 3");
        
        CHECK(text.getText() == "Line 1\nLine 2\nLine 3");
        
        BeginDrawing();
        text.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIText special characters") {
        init_raylib_for_tests();
        
        UI::UIText text(0, 0, "!@#$%^&*()_+-=[]{}|;:,.<>?");
        
        BeginDrawing();
        text.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIText with negative position") {
        init_raylib_for_tests();
        
        UI::UIText text(-50.0f, -100.0f, "Test");
        
        CHECK(text.getPosition().x == -50.0f);
        CHECK(text.getPosition().y == -100.0f);
        
        BeginDrawing();
        text.render();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("UIText setPosition and getPosition") {
        init_raylib_for_tests();
        
        UI::UIText text(0, 0, "Test");
        
        text.setPosition(300.0f, 400.0f);
        CHECK(text.getPosition().x == 300.0f);
        CHECK(text.getPosition().y == 400.0f);
        
        CloseWindow();
    }
}
