/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Loading
*/

#include "Loading.hpp"
#include "Core/States/GameStateManager.hpp"
#include <iostream>

Loading::Loading() : current_frame_(0), animation_timer_(0.0f), frame_duration_(1.0f / FRAMES_PER_SECOND)
{
    current_frame_texture_.id = 0;
}

Loading::~Loading()
{
    unload_current_frame();
}

void Loading::load_current_frame()
{
    // Unload previous frame if loaded
    unload_current_frame();

    // Load current frame (assuming files are named 0.png, 1.png, etc.)
    std::string frame_path = "Games/RType/Assets/joinusAnim/dedsecjoinus" + std::to_string(current_frame_) + ".png";
    current_frame_texture_ = LoadTexture(frame_path.c_str());

    if (current_frame_texture_.id == 0) {
        std::cerr << "Failed to load frame: " << frame_path << std::endl;
    }
}

void Loading::unload_current_frame()
{
    if (current_frame_texture_.id != 0 && IsWindowReady()) {
        UnloadTexture(current_frame_texture_);
        current_frame_texture_.id = 0;
    }
}

void Loading::enter()
{
    std::cout << "Entering Loading State" << std::endl;

    // Start with frame 0
    current_frame_ = 0;
    animation_timer_ = 0.0f;

    // Load the first frame
    load_current_frame();

    if (current_frame_texture_.id != 0) {
        std::cout << "Animation started with frame 0, size: " << current_frame_texture_.width << "x" << current_frame_texture_.height << std::endl;
    }
}

void Loading::exit()
{
    std::cout << "Exiting Loading State" << std::endl;

    // Unload current frame
    unload_current_frame();
}

void Loading::pause()
{
    // Nothing to do
}

void Loading::resume()
{
    // Nothing to do
}

void Loading::update(__attribute_maybe_unused__ float delta_time)
{
    // Update animation timer
    animation_timer_ += delta_time;

    // Advance frame when enough time elapsed
    if (animation_timer_ >= frame_duration_) {
        animation_timer_ -= frame_duration_;

        if (current_frame_ < TOTAL_FRAMES + 10) {
            ++current_frame_;
            load_current_frame();
        } else {
            if (_stateManager) {
                _stateManager->pop_state();
                _stateManager->change_state("MainMenu");
            }
        }
    }
}

void Loading::render()
{
    float scale = 2.8f;

    ClearBackground(BLACK);
    Vector2 position = {
        (GetScreenWidth() - current_frame_texture_.width * scale) / 2.0f,
        (GetScreenHeight() - current_frame_texture_.height * scale) / 2.0f
    };
    DrawTextureEx(current_frame_texture_, position, 0.0f, scale, WHITE);
    DrawText("Press SPACE to skip", 10, GetScreenHeight() - 30, 20, GRAY);
}

void Loading::handle_input()
{
    // Press any key to skip to MainMenu
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        if (_stateManager) {
            _stateManager->pop_state();
            _stateManager->push_state("MainMenu");
        }
    }
}
