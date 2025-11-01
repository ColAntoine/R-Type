/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** StarBg system implementation
*/

#include <memory>
#include <random>
#include <cmath>
#include <iostream>

#include "StarBg.hpp"
#include "Constants.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/Registry.hpp"

void StarBg::update(registry& r, float dt)
{
    // Initialize on first update (seed from registry)
    if (!initialized_) {
        seed_from_registry(r);
        initialized_ = true;
    }

    // Only generate stars once
    if (_stars.empty()) {
        generate_stars(r);
    }

    // Update star positions with actual delta time
    update_stars(dt, r);

    // Render stars
    render_stars();
}

void StarBg::seed_from_registry(registry& r) {
    if (r.has_random_seed()) {
        rng_.seed(r.get_random_seed());
        std::cout << "[StarBg] Seeded RNG with: " << r.get_random_seed() << std::endl;
    } else {
        // Fallback to random_device if no seed is set
        std::random_device rd;
        rng_.seed(rd());
        std::cout << "[StarBg] No registry seed found, using random seed" << std::endl;
    }
}

void StarBg::generate_stars(registry& r)
{
    auto& renderManager = RenderManager::instance();
    int screen_width = renderManager.get_screen_infos().getWidth();
    int screen_height = renderManager.get_screen_infos().getHeight();
    
    std::uniform_real_distribution<> dis_x(0.0, screen_width);
    std::uniform_real_distribution<> dis_y(0.0, screen_height);
    std::uniform_real_distribution<> dis_speed(50.0, 300.0);
    std::uniform_real_distribution<> dis_size(0.5, 3.0);
    std::uniform_int_distribution<> dis_brightness(200, 255);

    const int num_stars = 150;
    _stars.reserve(num_stars);

    for (int i = 0; i < num_stars; ++i) {
        Star star;
        star.x = dis_x(rng_);
        star.y = dis_y(rng_);
        star.speed = dis_speed(rng_);
        star.size = dis_size(rng_);
        star.brightness = static_cast<float>(dis_brightness(rng_));
        _stars.push_back(star);
    }
}

void StarBg::update_stars(float delta_time, registry& r)
{
    auto& renderManager = RenderManager::instance();
    int screen_width = renderManager.get_screen_infos().getWidth();
    int screen_height = renderManager.get_screen_infos().getHeight();
    
    std::uniform_real_distribution<> dis_y(0.0, screen_height);
    
    for (auto& star : _stars) {
        // Move star from right to left
        star.x -= star.speed * delta_time;

        // Wrap around: if star goes off left side, reset to right side
        if (star.x < -10.0f) {
            star.x = screen_width + 10.0f;
            star.y = dis_y(rng_);
        }
    }
}

void StarBg::render_stars() const
{
    auto& renderManager = RenderManager::instance();

    for (const auto& star : _stars) {
        // Create a subtle twinkling effect
        float flicker = 0.8f + 0.2f * sinf(GetTime() * 3.0f + star.x);
        Color star_color = {
            static_cast<unsigned char>(star.brightness * flicker),
            static_cast<unsigned char>(star.brightness * flicker),
            static_cast<unsigned char>(star.brightness * flicker * 0.9f),
            255
        };

        renderManager.draw_circle(static_cast<int>(star.x), static_cast<int>(star.y), star.size, star_color);
    }
}


DLL_EXPORT ISystem* create_system() {
    try {
        return new StarBg();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}
