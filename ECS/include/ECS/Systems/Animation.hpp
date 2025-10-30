/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** animation system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include <unordered_map>
#include <string>
#include <raylib.h>

struct AnimState {
    int current_frame{0};
    float elapsed{0.0f};
    int computed_frame_count{0};
};

class AnimationSystem : public ISystem {
    public:
        void update(registry& r, float dt = 0.0f) override;
        const char* get_name() const override { return "AnimationSystem"; }
        ~AnimationSystem();
    private:
        // runtime state per-entity (entity index -> state)
        std::unordered_map<std::size_t, AnimState> states_;
        // Texture cache to avoid reloading
        std::unordered_map<std::string, Texture2D> texture_cache_;
        
        Texture2D load_texture(const std::string& path);
};

#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);
