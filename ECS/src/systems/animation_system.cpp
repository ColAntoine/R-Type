/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** animation_system - Integrated with SpriteBatch for performance
*/

#include <raylib.h>
#include <iostream>
#include "ECS/Systems/Animation.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Renderer/RenderManager.hpp"

Texture2D AnimationSystem::load_texture(const std::string& path) {
    auto it = texture_cache_.find(path);
    if (it != texture_cache_.end()) {
        return it->second;
    }

    Texture2D texture = LoadTexture(path.c_str());
    if (texture.id == 0) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        // Create a default 1x1 white texture for missing textures
        Image img = GenImageColor(1, 1, WHITE);
        texture = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    texture_cache_[path] = texture;
    return texture;
}

void AnimationSystem::update(registry& r, float dt) {
    auto *anim_arr = r.get_if<animation>();
    auto *pos_arr = r.get_if<position>();
    // Animation system centers on collider if present, otherwise centers on pos

    if (!anim_arr || !pos_arr) return;

    for (auto [anim, pos, entity] : zipper(*anim_arr, *pos_arr)) {
        // Load texture to get dimensions if frame_count is not set
        if (anim.frame_count <= 0) {
            Texture2D texture = load_texture(anim.texture_path);
            if (texture.id != 0) {
                // Calculate frame count based on texture width and frame width
                anim.frame_count = static_cast<int>(texture.width / anim.frame_width);
            } else {
                continue; // Skip if texture failed to load
            }
        }

        // Update animation timer only if allowed by play_on_movement flag
        bool should_advance = true;
        if (anim.play_on_movement) {
            // If play_on_movement is enabled, check velocity component
            auto *vel_arr = r.get_if<velocity>();
            if (!vel_arr || !vel_arr->has(static_cast<size_t>(entity))) {
                should_advance = false;
            } else {
                auto &vel = vel_arr->get(static_cast<size_t>(entity));
                should_advance = (vel.vx != 0.0f || vel.vy != 0.0f);
            }
        }
        if (should_advance) {
            anim.frame_timer += dt;

            // Check if it's time to advance to the next frame
            if (anim.frame_timer >= anim.frame_time) {
                anim.frame_timer = 0.0f; // Reset timer

                // Advance to next frame
                anim.current_frame++;

                // Handle looping
                if (anim.current_frame >= anim.frame_count) {
                    if (anim.loop) {
                        anim.current_frame = 0; // Loop back to first frame
                    } else {
                        anim.current_frame = anim.frame_count - 1; // Stay on last frame
                    }
                }
            }
        }

        // Render the current frame
        Texture2D texture = load_texture(anim.texture_path);
        if (texture.id != 0) {
            // Calculate source rectangle for current frame
            Rectangle source = {
                (float)(anim.current_frame * anim.frame_width), // x position in sprite sheet
                0.0f, // y position (assuming horizontal sprite sheets)
                anim.frame_width,
                anim.frame_height
            };

            // Calculate scaled dimensions and always center animation at pos
            float scaled_width = anim.frame_width * anim.scale_x;
            float scaled_height = anim.frame_height * anim.scale_y;

            Rectangle dest = {
                pos.x - scaled_width / 2.0f,
                pos.y - scaled_height / 2.0f,
                scaled_width, scaled_height
            };

            Vector2 origin = {0.0f, 0.0f};

            // Use RenderManager wrapper to draw sprite
            RenderManager::instance().draw_sprite(&texture, source, dest, origin, 0.0f, WHITE, 0);
        }
    }
}

AnimationSystem::~AnimationSystem() {
    // Clear the texture cache without unloading textures
    // Raylib will handle texture cleanup when the graphics context is destroyed
    texture_cache_.clear();
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new AnimationSystem();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}