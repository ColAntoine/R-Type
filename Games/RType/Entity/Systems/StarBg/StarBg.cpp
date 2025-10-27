/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** StarBg system implementation
*/

#include <memory>
#include <random>
#include <cmath>

#include "StarBg.hpp"
#include "Constants.hpp"
#include "ECS/Renderer/RenderManager.hpp"

void StarBg::update(registry& r, float dt)
{
    // Only generate stars once
    if (_stars.empty()) {
        generate_stars();
    }

    // Update star positions with actual delta time
    update_stars(dt);

    // Render stars
    render_stars();
}

void StarBg::generate_stars()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    
    auto& renderManager = RenderManager::instance();
    int screen_width = renderManager.get_screen_infos().getWidth();
    int screen_height = renderManager.get_screen_infos().getHeight();
    
    std::uniform_real_distribution<> dis_y(0.0, screen_height);
    std::uniform_real_distribution<> dis_speed(50.0, 300.0);
    std::uniform_real_distribution<> dis_size(0.5, 3.0);

    const int num_stars = 150;
    _stars.reserve(num_stars);

    for (int i = 0; i < num_stars; ++i) {
        Star star;
        star.x = static_cast<float>(rand() % screen_width);
        star.y = dis_y(gen);
        star.speed = dis_speed(gen);
        star.size = dis_size(gen);
        star.brightness = 200.0f + (rand() % 55); // 200-255
        _stars.push_back(star);
    }
}

void StarBg::update_stars(float delta_time)
{
    auto& renderManager = RenderManager::instance();
    int screen_width = renderManager.get_screen_infos().getWidth();
    int screen_height = renderManager.get_screen_infos().getHeight();
    
    for (auto& star : _stars) {
        // Move star from right to left
        star.x -= star.speed * delta_time;

        // Wrap around: if star goes off left side, reset to right side
        if (star.x < -10.0f) {
            star.x = screen_width + 10.0f;
            std::uniform_real_distribution<> dis_y(0.0, screen_height);
            std::random_device rd;
            std::mt19937 gen(rd());
            star.y = dis_y(gen);
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


extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<StarBg>();
    }
}
