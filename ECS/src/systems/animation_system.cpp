/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** animation_system - Integrated with SpriteBatch for performance
*/

#include <raylib.h>
#include <iostream>
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/Systems/Animation.hpp"

static void killEnt(registry &r, std::vector<entity> ents)
{
    for (auto &ent: ents) {
        r.kill_entity(ent);
    }
}

Texture2D AnimationSystem::load_texture(const std::string& path) {
    auto it = texture_cache_.find(path);
    if (it != texture_cache_.end()) {
        return it->second;
    }

    Texture2D texture = LoadTexture(path.c_str());
    if (texture.id == 0) {
        std::cerr << "Failed to load texture: " << path << std::endl;
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
    std::vector<entity> entsToKill;

    if (!anim_arr || !pos_arr) return;

    for (auto [anim, pos, ent] : zipper(*anim_arr, *pos_arr)) {
        if (anim.frame_count <= 0) {
            Texture2D texture = load_texture(anim.texture_path);
            if (texture.id != 0) {
                anim.frame_count = static_cast<int>(texture.width / anim.frame_width);
            } else {
                continue;
            }
        }

        bool should_advance = shouldAdvance(anim, entity(ent), r);

        if (should_advance) {
            updateAnim(anim, dt, entsToKill, entity(ent));
        }
        renderAnim(anim, pos);
    }
    killEnt(r, entsToKill);
}

bool AnimationSystem::shouldAdvance(animation &anim, entity ent, registry &r)
{
    if (anim.play_on_movement) {
        auto *vel_arr = r.get_if<velocity>();
        if (!vel_arr || !vel_arr->has(static_cast<size_t>(ent))) {
            return false;
        } else {
            auto &vel = vel_arr->get(static_cast<size_t>(ent));
            return (vel.vx != 0.0f || vel.vy != 0.0f);
        }
    }
    return true;
}

void AnimationSystem::updateAnim(animation &anim, float dt, std::vector<entity> &entsToKill, entity ent)
{
    anim.frame_timer += dt;

    if (anim.frame_timer >= anim.frame_time) {
        anim.frame_timer = 0.0f;

        anim.current_frame++;

        if (anim.current_frame >= anim.frame_count) {
            if (anim.loop) {
                anim.current_frame = 0;
            } else {
                if (anim._stopAtTheEnd) {
                    entsToKill.push_back(ent);
                }
                anim.current_frame = std::max(0, anim.frame_count - 1);
            }
        }
    }
}

void AnimationSystem::renderAnim(animation &anim, position &pos)
{
    Texture2D texture = load_texture(anim.texture_path);
    if (texture.id != 0) {
        Rectangle source = {
            (float)(anim.current_frame * anim.frame_width),
            0.0f,
            anim.frame_width,
            anim.frame_height
        };

        float scaled_width = anim.frame_width * anim.scale_x;
        float scaled_height = anim.frame_height * anim.scale_y;

        Rectangle dest = {
            pos.x - scaled_width / 2.0f,
            pos.y - scaled_height / 2.0f,
            scaled_width, scaled_height
        };

        Vector2 origin = {0.0f, 0.0f};

        RenderManager::instance().draw_sprite(&texture, source, dest, origin, 0.0f, WHITE, 0);
    }
}

AnimationSystem::~AnimationSystem() {
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