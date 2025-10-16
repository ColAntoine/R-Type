/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI System Test
*/

#include <iostream>
#include <raylib.h>
#include "ECS/Registry.hpp"
#include "ECS/Components/UIComponent.hpp"
#include "ECS/UI/Components/Button.hpp"
#include "ECS/Systems/UISystem.hpp"

// int main() {
//     // Initialize window
//     InitWindow(800, 600, "UI System Test");
//     SetTargetFPS(60);

//     // Create registry and UI system
//     registry reg;
//     UI::UISystem ui_system;

//     // Register UIComponent
//     reg.register_component<UI::UIComponent>();

//     // Create a button entity
//     auto button_entity = reg.spawn_entity();
    
//     // Create button
//     auto play_button = std::make_shared<UI::UIButton>(300, 250, 200, 60, "Play Game");
    
//     // Set button style
//     UI::ButtonStyle style;
//     style._normal_color = {70, 70, 70, 255};
//     style._hovered_color = {100, 100, 255, 255};
//     style._pressed_color = {50, 50, 150, 255};
//     style._font_size = 24;
//     play_button->set_style(style);
    
//     // Set button callback
//     play_button->set_on_click([]() {
//         std::cout << "Play button clicked!" << std::endl;
//     });
    
//     play_button->set_on_hover([]() {
//         std::cout << "Hovering over play button" << std::endl;
//     });
    
//     play_button->set_on_hover_exit([]() {
//         std::cout << "Stopped hovering" << std::endl;
//     });

//     // Add button to registry
//     reg.add_component(button_entity, UI::UIComponent(play_button));

//     // Create quit button
//     auto quit_button_entity = reg.spawn_entity();
//     auto quit_button = std::make_shared<UI::UIButton>(300, 330, 200, 60, "Quit");
//     quit_button->set_style(style);
//     quit_button->set_on_click([]() {
//         std::cout << "Quit button clicked! Closing window..." << std::endl;
//     });
//     reg.add_component(quit_button_entity, UI::UIComponent(quit_button));

//     // Main game loop
//     while (!WindowShouldClose()) {
//         float deltaTime = GetFrameTime();

//         // Process input
//         ui_system.process_input(reg);

//         // Update
//         ui_system.update(reg, deltaTime);

//         // Render
//         BeginDrawing();
//         ClearBackground(BLACK);
        
//         DrawText("UI System Test", 250, 100, 40, WHITE);
//         DrawText("Hover and click the buttons!", 220, 450, 20, GRAY);
        
//         ui_system.render(reg);
        
//         EndDrawing();
//     }

//     CloseWindow();
//     return 0;
// }
