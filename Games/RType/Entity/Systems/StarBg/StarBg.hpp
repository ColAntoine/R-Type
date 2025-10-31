/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** StarBg system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

#include <vector>
#include <random>

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
    std::mt19937 rng_;
    bool initialized_{false};

    void generate_stars(registry& r);
    void seed_from_registry(registry& r);
    void update_stars(float delta_time, registry& r);
    void render_stars() const;
};


#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);
