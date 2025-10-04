/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI Components Demo - Test our UI components
*/

#pragma once

#include "ui/ui_manager.hpp"
#include "ui/components/ui_button.hpp"
#include "ui/components/ui_text.hpp"
#include "ui/components/ui_panel.hpp"
#include "ui/components/ui_input_field.hpp"

class UIDemo {
    private:
        UIManager ui_manager_;
        bool demo_running_{true};

    public:
        UIDemo();
        ~UIDemo() = default;

        void run();
        void update(float delta_time);
        void render();
        void handle_input();

    private:
        void setup_demo_ui();
        void setup_main_menu();
        void setup_settings_panel();

        // Callback functions
        void on_play_button_clicked();
        void on_settings_button_clicked();
        void on_quit_button_clicked();
        void on_back_button_clicked();
        void on_name_changed(const std::string& name);
};