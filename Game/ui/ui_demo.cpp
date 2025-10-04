/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI Components Demo Implementation
*/

#include "ui_demo.hpp"
#include <iostream>

UIDemo::UIDemo() {
    setup_demo_ui();
}

void UIDemo::run() {
    // This would typically be called from the main game loop
    // For now, we'll just set up the components
    std::cout << "UI Demo initialized with " << ui_manager_.get_component_count() << " components" << std::endl;
}

void UIDemo::update(float delta_time) {
    ui_manager_.update(delta_time);
}

void UIDemo::render() {
    ui_manager_.render();
}

void UIDemo::handle_input() {
    ui_manager_.handle_input();

    // Check for escape to quit demo
    if (IsKeyPressed(KEY_ESCAPE)) {
        demo_running_ = false;
    }
}

void UIDemo::setup_demo_ui() {
    setup_main_menu();
    setup_settings_panel();
}

void UIDemo::setup_main_menu() {
    // Create main menu panel
    auto main_panel = std::make_shared<UIPanel>(100, 100, 400, 500);
    main_panel->set_background_color({30, 30, 30, 240});
    main_panel->set_border_color({100, 100, 100, 255});
    ui_manager_.add_component("main_panel", main_panel);

    // Game title
    auto title = std::make_shared<UIText>(150, 130, "R-TYPE GAME", 36);
    title->set_text_color({255, 255, 255, 255});
    title->set_alignment(TextAlignment::Center);
    title->set_size(300, 50); // Set size for center alignment
    ui_manager_.add_component("title", title);

    // Play button
    auto play_button = std::make_shared<UIButton>(150, 220, 200, 50, "PLAY");
    play_button->set_colors(
        {70, 130, 180, 255},  // Normal - Steel blue
        {100, 160, 210, 255}, // Hovered
        {50, 110, 160, 255},  // Pressed
        {40, 40, 40, 255}     // Disabled
    );
    play_button->set_on_click([this]() { on_play_button_clicked(); });
    ui_manager_.add_component("play_button", play_button);

    // Settings button
    auto settings_button = std::make_shared<UIButton>(150, 290, 200, 50, "SETTINGS");
    settings_button->set_colors(
        {100, 100, 100, 255}, // Normal - Gray
        {130, 130, 130, 255}, // Hovered
        {80, 80, 80, 255},    // Pressed
        {40, 40, 40, 255}     // Disabled
    );
    settings_button->set_on_click([this]() { on_settings_button_clicked(); });
    ui_manager_.add_component("settings_button", settings_button);

    // Quit button
    auto quit_button = std::make_shared<UIButton>(150, 360, 200, 50, "QUIT");
    quit_button->set_colors(
        {180, 70, 70, 255},   // Normal - Red
        {210, 100, 100, 255}, // Hovered
        {160, 50, 50, 255},   // Pressed
        {40, 40, 40, 255}     // Disabled
    );
    quit_button->set_on_click([this]() { on_quit_button_clicked(); });
    ui_manager_.add_component("quit_button", quit_button);
}

void UIDemo::setup_settings_panel() {
    // Settings panel (initially hidden)
    auto settings_panel = std::make_shared<UIPanel>(150, 150, 500, 400);
    settings_panel->set_background_color({40, 40, 40, 250});
    settings_panel->set_border_color({120, 120, 120, 255});
    settings_panel->set_visible(false);
    ui_manager_.add_component("settings_panel", settings_panel);

    // Settings title
    auto settings_title = std::make_shared<UIText>(200, 180, "SETTINGS", 28);
    settings_title->set_text_color({255, 255, 255, 255});
    settings_title->set_visible(false);
    ui_manager_.add_component("settings_title", settings_title);

    // Player name input
    auto name_label = std::make_shared<UIText>(200, 230, "Player Name:", 20);
    name_label->set_text_color({200, 200, 200, 255});
    name_label->set_visible(false);
    ui_manager_.add_component("name_label", name_label);

    auto name_input = std::make_shared<UIInputField>(200, 255, 250, 35, "Enter your name...");
    name_input->set_colors(
        {60, 60, 60, 255},    // Background
        {80, 80, 80, 255},    // Focused background
        {100, 100, 100, 255}, // Border
        {150, 150, 255, 255}  // Focused border
    );
    name_input->set_on_text_changed([this](const std::string& name) { on_name_changed(name); });
    name_input->set_visible(false);
    ui_manager_.add_component("name_input", name_input);

    // Back button
    auto back_button = std::make_shared<UIButton>(200, 450, 150, 40, "BACK");
    back_button->set_colors(
        {100, 100, 100, 255}, // Normal
        {130, 130, 130, 255}, // Hovered
        {80, 80, 80, 255},    // Pressed
        {40, 40, 40, 255}     // Disabled
    );
    back_button->set_on_click([this]() { on_back_button_clicked(); });
    back_button->set_visible(false);
    ui_manager_.add_component("back_button", back_button);
}

void UIDemo::on_play_button_clicked() {
    std::cout << "Play button clicked! Starting game..." << std::endl;
    // Here you would transition to the game state
}

void UIDemo::on_settings_button_clicked() {
    std::cout << "Settings button clicked! Opening settings..." << std::endl;

    // Hide main menu
    ui_manager_.get_component("main_panel")->set_visible(false);
    ui_manager_.get_component("title")->set_visible(false);
    ui_manager_.get_component("play_button")->set_visible(false);
    ui_manager_.get_component("settings_button")->set_visible(false);
    ui_manager_.get_component("quit_button")->set_visible(false);

    // Show settings menu
    ui_manager_.get_component("settings_panel")->set_visible(true);
    ui_manager_.get_component("settings_title")->set_visible(true);
    ui_manager_.get_component("name_label")->set_visible(true);
    ui_manager_.get_component("name_input")->set_visible(true);
    ui_manager_.get_component("back_button")->set_visible(true);
}

void UIDemo::on_quit_button_clicked() {
    std::cout << "Quit button clicked! Exiting application..." << std::endl;
    demo_running_ = false;
}

void UIDemo::on_back_button_clicked() {
    std::cout << "Back button clicked! Returning to main menu..." << std::endl;

    // Hide settings menu
    ui_manager_.get_component("settings_panel")->set_visible(false);
    ui_manager_.get_component("settings_title")->set_visible(false);
    ui_manager_.get_component("name_label")->set_visible(false);
    ui_manager_.get_component("name_input")->set_visible(false);
    ui_manager_.get_component("back_button")->set_visible(false);

    // Show main menu
    ui_manager_.get_component("main_panel")->set_visible(true);
    ui_manager_.get_component("title")->set_visible(true);
    ui_manager_.get_component("play_button")->set_visible(true);
    ui_manager_.get_component("settings_button")->set_visible(true);
    ui_manager_.get_component("quit_button")->set_visible(true);
}

void UIDemo::on_name_changed(const std::string& name) {
    std::cout << "Player name changed to: " << name << std::endl;
}