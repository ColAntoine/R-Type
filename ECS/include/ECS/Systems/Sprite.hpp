/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** sprite_system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include <unordered_map>
#include <raylib.h>

class SpriteRenderSystem : public ISystem {
    private:
        std::unordered_map<std::string, Texture2D> texture_cache_;

        Texture2D get_default_texture();
        Texture2D load_texture(const std::string& path);

    public:
        void update(registry& r, float dt = 0.0f) override;
        const char* get_name() const override { return "SpriteRenderSystem"; }
        ~SpriteRenderSystem();
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
