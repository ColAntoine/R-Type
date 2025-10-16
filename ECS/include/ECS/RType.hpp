/*
** EPITECH PROJECT, 2025
** asdasd
** File description:
** rtype
*/

// TODO: Remove this file

#pragma once

#include <algorithm>
#include <raylib.h>

#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/ILoader.hpp"

class RType {
  public:
    RType();
    ~RType() = default;

    int run();

  private:
    void load_systems();
    void setup_scene();
    void handle_events();
    void update(float dt);
    void render();

    void create_player(IComponentFactory* factory, float x, float y, float w, float h, float speed);
    void create_static_entity(IComponentFactory* factory, float x, float y, float w, float h,
                             uint8_t r, uint8_t g, uint8_t b, bool has_collider = false);
    void create_moving_entity(IComponentFactory* factory, float x, float y, float w, float h,
                            float vx, float vy, uint8_t r, uint8_t g, uint8_t b);

  private:
    registry reg_;
    std::unique_ptr<ILoader> loader_;
};