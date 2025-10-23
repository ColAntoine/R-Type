// /*
// ** EPITECH PROJECT, 2025
// ** R-Type
// ** File description:
// ** UI System Test
// */

// #include <iostream>
// #include <cmath>
// #include <raylib.h>
// #include "ECS/Registry.hpp"
// #include "ECS/Components/UIComponent.hpp"
// #include "ECS/UI/Components/Button.hpp"
// #include "ECS/UI/Components/InputField.hpp"
// #include "ECS/UI/Components/Panel.hpp"
// #include "ECS/UI/Components/Text.hpp"
// #include "ECS/Systems/UISystem.hpp"

// // Custom button with glitch effect on hover
// class GlitchButton : public UI::UIButton {
// public:
//     GlitchButton(float x, float y, float w, float h, const std::string& text)
//         : UIButton(x, y, w, h, text) {
//         hover_seed_ = rand() % 1000;
//     }

//     void set_glitch_params(float amplitude, float speed, Color neon, Color glow) {
//         hover_jitter_amplitude_ = amplitude;
//         hover_jitter_speed_ = speed;
//         neon_color_ = neon;
//         neon_glow_color_ = glow;
//     }

// protected:
//     void drawButtonBackground() const override {
//         Color bg_color = get_current_background_color();
//         Vector2 pos = get_position();
//         Vector2 size = get_size();
        
//         // Draw main button rectangle
//         DrawRectangle(pos.x, pos.y, size.x, size.y, bg_color);
        
//         // Draw border
//         Color border_color = {
//             static_cast<unsigned char>(bg_color.r + 30),
//             static_cast<unsigned char>(bg_color.g + 30),
//             static_cast<unsigned char>(bg_color.b + 30),
//             255
//         };
//         DrawRectangleLines(pos.x, pos.y, size.x, size.y, border_color);
        
//         // Add scanline texture for cyberpunk feel
//         for (int i = 0; i < static_cast<int>(size.y); i += 6) {
//             DrawRectangle(pos.x, pos.y + i, size.x, 1, {0, 0, 0, 10});
//         }
        
//         // Add neon glow when hovered
//         if (get_state() == UI::UIState::Hovered) {
//             DrawRectangleLinesEx(
//                 {pos.x - 2, pos.y - 2, size.x + 4, size.y + 4},
//                 2.0f,
//                 neon_color_
//             );
//             DrawRectangle(
//                 pos.x - 4,
//                 pos.y - 4,
//                 size.x + 8,
//                 4,
//                 neon_glow_color_
//             );
//         }
        
//         // Pressed state highlight
//         if (get_state() == UI::UIState::Pressed) {
//             DrawRectangle(
//                 pos.x + 2,
//                 pos.y + 2,
//                 size.x - 4,
//                 size.y - 4,
//                 {255, 255, 255, 30}
//             );
//         }
//     }

//     void drawButtonText() const override {
//         if (get_text().empty()) return;

//         Vector2 pos = get_position();
//         Vector2 size = get_size();
//         const auto& style = get_style();
//         Color text_color = (get_state() == UI::UIState::Disabled) 
//             ? style._disabled_text_color 
//             : style._text_color;
        
//         Vector2 text_size = MeasureTextEx(GetFontDefault(), get_text().c_str(), style._font_size, 1.0f);
//         float text_x = pos.x + (size.x - text_size.x) / 2.0f;
//         float text_y = pos.y + (size.y - text_size.y) / 2.0f;

//         // Glitch effect on hover
//         if (get_state() == UI::UIState::Hovered) {
//             float t = GetTime() * hover_jitter_speed_ + hover_seed_;
//             float jitter_x = std::sin(t) * hover_jitter_amplitude_;
//             float jitter_y = std::cos(t * 1.3f) * (hover_jitter_amplitude_ / 2.0f);

//             // RGB split effect
//             Color rcol = {255, 50, 80, text_color.a};
//             DrawTextEx(GetFontDefault(), get_text().c_str(),
//                       {text_x + jitter_x + 1, text_y + jitter_y},
//                       style._font_size, 1.0f, rcol);

//             Color gcol = {0, 255, 156, static_cast<unsigned char>(text_color.a * 0.6f)};
//             DrawTextEx(GetFontDefault(), get_text().c_str(),
//                       {text_x - jitter_x - 1, text_y - jitter_y},
//                       style._font_size, 1.0f, gcol);

//             DrawTextEx(GetFontDefault(), get_text().c_str(),
//                       {text_x, text_y}, style._font_size, 1.0f, text_color);

//             // Scanline flicker
//             if (((static_cast<int>(GetTime() * 10) + hover_seed_) % 5) == 0) {
//                 DrawRectangle(text_x - 4, text_y + text_size.y + 2,
//                             text_size.x + 8, 2, {255, 255, 255, 10});
//             }
//         } else {
//             // Normal rendering
//             if (get_state() == UI::UIState::Pressed) {
//                 text_x += 1;
//                 text_y += 1;
//             }
//             DrawTextEx(GetFontDefault(), get_text().c_str(),
//                       {text_x, text_y}, style._font_size, 1.0f, text_color);
//         }
//     }

// private:
//     Color neon_color_{0, 229, 255, 255};
//     Color neon_glow_color_{0, 229, 255, 100};
//     float hover_jitter_amplitude_{2.0f};
//     float hover_jitter_speed_{8.0f};
//     int hover_seed_{0};
// };

// int main() {
//     // Initialize window
//     InitWindow(800, 600, "UI System Test");
//     SetTargetFPS(60);

//     // Create registry and UI system
//     registry reg;
//     UI::UISystem ui_system;

//     // Register UIComponent
//     reg.register_component<UI::UIComponent>();

//     // ========================================
//     // STEP 1: Create background panel FIRST (rendered behind everything)
//     // ========================================
//     auto panel_entity = reg.spawn_entity();
//     auto background_panel = std::make_shared<UI::UIPanel>(200, 120, 400, 420);
    
//     // Set panel style
//     UI::PanelStyle panel_style;
//     panel_style._background_color = {30, 30, 40, 220};  // Semi-transparent dark blue
//     panel_style._border_color = {0, 229, 255, 255};     // Cyan border
//     panel_style._border_thickness = 2.0f;
//     panel_style._corner_radius = 10.0f;                 // Rounded corners
//     panel_style._has_shadow = true;
//     panel_style._shadow_color = {0, 0, 0, 150};
//     panel_style._shadow_offset = 8.0f;
//     background_panel->set_style(panel_style);
    
//     // Add panel to registry FIRST
//     reg.add_component(panel_entity, UI::UIComponent(background_panel));

//     // ========================================
//     // STEP 2: Create title text
//     // ========================================
//     auto title_entity = reg.spawn_entity();
//     auto title_text = std::make_shared<UI::UIText>(400, 140, "UI SYSTEM TEST");
    
//     // Set text style
//     UI::TextStyle title_style;
//     title_style._text_color = {0, 229, 255, 255};  // Cyan
//     title_style._font_size = 36;
//     title_style._alignment = UI::TextAlignment::Center;
//     title_style._has_shadow = true;
//     title_style._shadow_color = {0, 0, 0, 200};
//     title_style._shadow_offset = {3.0f, 3.0f};
//     title_text->set_style(title_style);
    
//     // Add title to registry
//     reg.add_component(title_entity, UI::UIComponent(title_text));

//     // ========================================
//     // STEP 3: Create username label
//     // ========================================
//     auto label_entity = reg.spawn_entity();
//     auto username_label = std::make_shared<UI::UIText>(250, 195, "Username:");
    
//     UI::TextStyle label_style;
//     label_style._text_color = {200, 200, 200, 255};
//     label_style._font_size = 18;
//     label_style._alignment = UI::TextAlignment::Left;
//     username_label->set_style(label_style);
    
//     reg.add_component(label_entity, UI::UIComponent(username_label));

//     // ========================================
//     // STEP 4: Create input field
//     // ========================================
//     auto input_entity = reg.spawn_entity();
//     auto username_input = std::make_shared<UI::UIInputField>(250, 220, 300, 50, "Enter username...");
    
//     // Set input field style
//     UI::InputFieldStyle input_style;
//     input_style._background_color = {50, 50, 50, 255};
//     input_style._focused_color = {70, 70, 90, 255};
//     input_style._border_color = {100, 100, 100, 255};
//     input_style._focused_border_color = {0, 229, 255, 255};  // Cyan when focused
//     input_style._text_color = {255, 255, 255, 255};
//     input_style._placeholder_color = {150, 150, 150, 255};
//     input_style._cursor_color = {0, 229, 255, 255};
//     input_style._font_size = 20;
//     input_style._border_thickness = 2.0f;
//     username_input->set_style(input_style);
    
//     // Set max length
//     username_input->set_max_length(20);
    
//     // Set callbacks
//     username_input->set_on_text_changed([](const std::string& text) {
//         std::cout << "Text changed: " << text << std::endl;
//     });
    
//     username_input->set_on_enter_pressed([](const std::string& text) {
//         std::cout << "Enter pressed! Username: " << text << std::endl;
//     });
    
//     username_input->set_on_focus([]() {
//         std::cout << "Input field focused" << std::endl;
//     });
    
//     username_input->set_on_focus_lost([]() {
//         std::cout << "Input field lost focus" << std::endl;
//     });
    
//     // Add input field to registry
//     reg.add_component(input_entity, UI::UIComponent(username_input));

//     // ========================================
//     // STEP 5: Create buttons
//     // ========================================
//     // Create button style
//     UI::ButtonStyle style;
//     style._normal_color = {70, 70, 70, 255};
//     style._hovered_color = {100, 100, 255, 255};
//     style._pressed_color = {50, 50, 150, 255};
//     style._font_size = 24;

//     // Create a glitch button entity
//     auto button_entity = reg.spawn_entity();
    
//     // Create glitch button with cyberpunk effect
//     auto play_button = std::make_shared<GlitchButton>(300, 290, 200, 60, "Play Game");
//     play_button->set_style(style);
    
//     // Configure glitch parameters (amplitude, speed, neon color, glow color)
//     play_button->set_glitch_params(
//         2.0f,                      // Jitter amplitude
//         8.0f,                      // Jitter speed
//         {0, 229, 255, 255},       // Neon cyan
//         {0, 229, 255, 100}        // Neon glow
//     );
    
//     // Set button callbacks
//     play_button->set_on_click([]() {
//         std::cout << "Play button clicked!" << std::endl;
//     });
    
//     play_button->set_on_hover([]() {
//         std::cout << "Hovering over play button - GLITCH ACTIVE" << std::endl;
//     });
    
//     play_button->set_on_hover_exit([]() {
//         std::cout << "Stopped hovering" << std::endl;
//     });

//     // Add button to registry
//     reg.add_component(button_entity, UI::UIComponent(play_button));

//     // Create quit button with glitch effect
//     auto quit_button_entity = reg.spawn_entity();
//     auto quit_button = std::make_shared<GlitchButton>(300, 370, 200, 60, "Quit");
    
//     // Different color scheme for quit button
//     UI::ButtonStyle quit_style = style;
//     quit_style._hovered_color = {255, 50, 50, 255};  // Red hover
//     quit_button->set_style(quit_style);
    
//     // Configure with red neon
//     quit_button->set_glitch_params(
//         2.5f,                      // Slightly more jitter
//         10.0f,                     // Faster speed
//         {255, 0, 100, 255},       // Red neon
//         {255, 0, 100, 100}        // Red glow
//     );
    
//     quit_button->set_on_click([]() {
//         std::cout << "Quit button clicked! Closing window..." << std::endl;
//     });
    
//     quit_button->set_on_hover([]() {
//         std::cout << "Hovering over quit - WARNING GLITCH" << std::endl;
//     });
    
//     reg.add_component(quit_button_entity, UI::UIComponent(quit_button));

//     // ========================================
//     // STEP 6: Create footer text
//     // ========================================
//     auto footer_entity = reg.spawn_entity();
//     auto footer_text = std::make_shared<UI::UIText>(400, 510, "Press Enter to submit | ESC to quit");
    
//     UI::TextStyle footer_style;
//     footer_style._text_color = {100, 100, 100, 255};
//     footer_style._font_size = 14;
//     footer_style._alignment = UI::TextAlignment::Center;
//     footer_text->set_style(footer_style);
    
//     reg.add_component(footer_entity, UI::UIComponent(footer_text));

//     // Main game loop
//     while (!WindowShouldClose()) {
//         float deltaTime = GetFrameTime();

//         // Process input
//         ui_system.process_input(reg);

//         // Update
//         ui_system.update(reg, deltaTime);

//         // Render
//         BeginDrawing();
//         ClearBackground({10, 10, 15, 255});  // Dark background
        
//         // All UI elements are now rendered through the UI system
//         ui_system.render(reg);
        
//         EndDrawing();
//     }

//     CloseWindow();
//     return 0;
// }
