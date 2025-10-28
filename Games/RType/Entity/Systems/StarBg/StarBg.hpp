/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** StarBg system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

#include <vector>

struct Star {
    float x;
    float y;
    float speed;
    float size;
    float brightness;
};

class StarBg : public ISystem {
public:
    ~StarBg() override = default;
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "StarBg"; }
private:
    std::vector<Star> _stars;

    void generate_stars();
    void update_stars(float delta_time);
    void render_stars() const;
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
